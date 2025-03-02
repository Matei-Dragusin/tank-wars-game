#include "lab_m1/Tema1/tank.h"
#include "lab_m1/Tema1/terrain.h"
#include "lab_m1/Tema1/projectile.h"
#include "lab_m1/Tema1/Tema1.h"
#include "utils/gl_utils.h"
#include "core/engine.h"
#include <iostream>

using namespace m1;

Tank::Tank(const glm::vec2 &startPosition, const glm::vec3 &tankColor)
    : position(startPosition),
      color(tankColor),
      rotation(0.0f),
      turretRotation(0.0f),
      health(MAX_HEALTH),
      lastFireTime(0.0f),
      power(1.0f),
      terrain(nullptr)
{
}

void Tank::Move(float direction, float deltaTime)
{
    if (!IsAlive() || deltaTime <= 0.0f || !terrain)
        return;

    float newX = position.x + direction * MAX_MOVE_SPEED * deltaTime;

    // Limit area to move
    const float MARGIN = 40.0f;
    newX = glm::clamp(newX, MARGIN, 1280.0f - MARGIN);

    float currentHeight = terrain->GetHeightAtPosition(position.x);
    float nextHeight = terrain->GetHeightAtPosition(newX);

    const float MAX_SLOPE_ANGLE = glm::radians(60.0f);
    const float SLOPE_SPEED_FACTOR = 0.7f;
    const float DOWNHILL_SPEED_BOOST = 1.3f;

    float dx = newX - position.x;
    float dy = nextHeight - currentHeight;
    float slope = atan2(dy, abs(dx));

    float normalizedSlope = slope / MAX_SLOPE_ANGLE;
    float speedMultiplier = 1.0f;

    if (direction * dx > 0)
    {
        if (slope > 0)
        {
            speedMultiplier = 1.0f - SLOPE_SPEED_FACTOR * normalizedSlope;
        }
        else
        {
            speedMultiplier = 1.0f + DOWNHILL_SPEED_BOOST * (-normalizedSlope);
        }
    }
    else
    {
        if (slope > 0)
        {
            speedMultiplier = 1.0f + DOWNHILL_SPEED_BOOST * normalizedSlope;
        }
        else
        {
            speedMultiplier = 1.0f - SLOPE_SPEED_FACTOR * (-normalizedSlope);
        }
    }

    speedMultiplier = glm::clamp(speedMultiplier, 0.3f, 1.8f);

    if (slope > MAX_SLOPE_ANGLE)
    {
        if ((slope > 0 && direction > 0) || (slope < 0 && direction < 0))
        {
            return;
        }
        speedMultiplier *= 0.5f;
    }

    float adjustedSpeed = MAX_MOVE_SPEED * speedMultiplier;
    float finalX = position.x + direction * adjustedSpeed * deltaTime;
    finalX = glm::clamp(finalX, MARGIN, 1280.0f - MARGIN);

    position.x = finalX;
    float finalHeight = terrain->GetHeightAtPosition(finalX);
    UpdatePosition(finalHeight);
}

void Tank::RotateTurret(float direction, float deltaTime)
{
    if (!IsAlive() || deltaTime <= 0.0f)
        return;

    float rotationAmount = direction * TURRET_ROTATION_SPEED * deltaTime;
    float currentDegrees = glm::degrees(turretRotation);
    float newDegrees = currentDegrees + rotationAmount;

    newDegrees = glm::clamp(newDegrees, MIN_TURRET_ANGLE, MAX_TURRET_ANGLE);
    turretRotation = glm::radians(newDegrees);
}

void Tank::IncreasePower()
{
    if (!IsAlive())
        return;
    power = glm::min(power + POWER_STEP, MAX_POWER);
}

void Tank::DecreasePower()
{
    if (!IsAlive())
        return;
    power = glm::max(power - POWER_STEP, MIN_POWER);
}

bool Tank::CanFire() const
{
    float currentTime = Engine::GetElapsedTime();
    return IsAlive() && (currentTime - lastFireTime >= FIRE_COOLDOWN);
}

glm::vec2 Tank::GetFiringDirection() const
{
    float totalRotation = rotation + turretRotation + glm::radians(90.0f);
    return glm::normalize(glm::vec2(cos(totalRotation), sin(totalRotation)));
}

glm::vec2 Tank::GetBarrelTipPosition() const
{
    float baseRotation = rotation;
    float totalRotation = baseRotation + turretRotation + glm::radians(90.0f);

    glm::vec2 rotatedOffset = glm::vec2(
        -TURRET_CENTER_OFFSET * sin(baseRotation),
        TURRET_CENTER_OFFSET * cos(baseRotation));

    glm::vec2 turretCenter = position + rotatedOffset;
    glm::vec2 barrelDirection = glm::vec2(cos(totalRotation), sin(totalRotation));
    return turretCenter + barrelDirection * BARREL_LENGTH;
}

Projectile *Tank::Fire()
{
    if (!CanFire())
        return nullptr;

    try
    {
        lastFireTime = Engine::GetElapsedTime();
        glm::vec2 direction = GetFiringDirection();
        glm::vec2 barrelTip = GetBarrelTipPosition();
        glm::vec2 velocity = direction * (BASE_PROJECTILE_SPEED * power);

        return new Projectile(barrelTip, velocity);
    }
    catch (const std::exception &e)
    {
        std::cout << "Error creating projectile: " << e.what() << std::endl;
        return nullptr;
    }
    catch (...)
    {
        std::cout << "Unknown error creating projectile" << std::endl;
        return nullptr;
    }
}

void Tank::RenderTank(Tema1 *game)
{
    if (!IsAlive())
        return;

    // 1. Tank body
    glm::mat4 bodyModelMatrix = glm::mat4(1);
    bodyModelMatrix = glm::translate(bodyModelMatrix, glm::vec3(position, 0));
    bodyModelMatrix = glm::rotate(bodyModelMatrix, rotation, glm::vec3(0, 0, 1));
    game->RenderMesh(game->meshes["tank_body"], game->shaders["VertexColor"], bodyModelMatrix);

    // 2. Turret base (sphere) - uses the same transformations as the body
    glm::mat4 turretModelMatrix = bodyModelMatrix;
    game->RenderMesh(game->meshes["tank_turret_base"], game->shaders["VertexColor"], turretModelMatrix);

    // 3. Barrel - needs to rotate around the center of the turret
    glm::mat4 barrelModelMatrix = bodyModelMatrix;
    // Translate to the center of the turret
    barrelModelMatrix = glm::translate(barrelModelMatrix, glm::vec3(0, TURRET_CENTER_OFFSET, 0));
    // Rotate the barrel
    barrelModelMatrix = glm::rotate(barrelModelMatrix, turretRotation + glm::radians(180.0f), glm::vec3(0, 0, 1));
    // Translate back
    barrelModelMatrix = glm::translate(barrelModelMatrix, glm::vec3(0, -TURRET_CENTER_OFFSET, 0));
    game->RenderMesh(game->meshes["tank_barrel"], game->shaders["VertexColor"], barrelModelMatrix);

    const float HEALTH_BAR_OFFSET = 65.0f;
    float healthPercent = health / MAX_HEALTH;
    glm::mat4 healthMatrix = glm::mat4(1);
    healthMatrix = glm::translate(healthMatrix,
                                  glm::vec3(position.x - 40, position.y + HEALTH_BAR_OFFSET, 0));
    healthMatrix = glm::scale(healthMatrix, glm::vec3(healthPercent, 1, 1));

    game->RenderMesh(game->meshes["health_bar"], game->shaders["VertexColor"], healthMatrix);
}

void Tank::RenderTrajectory(Tema1 *game)
{
    if (!IsAlive())
        return;

    std::vector<glm::vec2> trajectoryPoints = CalculateTrajectory();
    if (trajectoryPoints.empty())
        return;

    std::vector<VertexFormat> vertices;
    glm::vec3 trajectoryColor(0, 0, 0);

    for (size_t i = 0; i < trajectoryPoints.size(); i++)
    {
        vertices.push_back(VertexFormat(glm::vec3(trajectoryPoints[i], 0), trajectoryColor));
    }

    std::vector<unsigned int> indices;
    for (size_t i = 0; i < vertices.size() - 1; i++)
    {
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    auto trajectoryMesh = game->meshes["trajectory_line"];
    trajectoryMesh->SetDrawMode(GL_LINES);
    trajectoryMesh->InitFromData(vertices, indices);

    glLineWidth(3.0f);
    game->RenderMesh(trajectoryMesh, game->shaders["VertexColor"], glm::mat4(1));
    glLineWidth(1.0f);
}

void Tank::TakeDamage(float damage)
{
    if (!IsAlive())
        return;

    health = std::max(0.0f, health - damage);
}

std::vector<glm::vec2> Tank::CalculateTrajectory() const
{
    std::vector<glm::vec2> trajectory;
    if (!IsAlive() || !terrain)
        return trajectory;

    const int NUM_POINTS = 500;
    const float TIME_STEP = 0.033f;
    const float GRAVITY = 981.0f;

    glm::vec2 startPos = GetBarrelTipPosition();
    glm::vec2 velocity = GetFiringDirection() * (BASE_PROJECTILE_SPEED * power);

    trajectory.push_back(startPos);

    glm::vec2 pos = startPos;
    glm::vec2 vel = velocity;

    for (int i = 1; i < NUM_POINTS; i++)
    {
        vel.y -= GRAVITY * TIME_STEP;
        glm::vec2 nextPos = pos + vel * TIME_STEP;

        if (nextPos.x < 0 || nextPos.x > 1280)
        {
            break;
        }

        float terrainHeight = terrain->GetHeightAtPosition(nextPos.x);
        if (nextPos.y <= terrainHeight)
        {
            float t = 0.0f;
            glm::vec2 dir = nextPos - pos;
            float dx = nextPos.x - pos.x;

            for (float step = 0.0f; step <= 1.0f; step += 0.1f)
            {
                glm::vec2 testPoint = pos + dir * step;
                if (testPoint.y <= terrain->GetHeightAtPosition(testPoint.x))
                {
                    trajectory.push_back(pos + dir * (step - 0.1f));
                    return trajectory;
                }
            }
            break;
        }

        trajectory.push_back(nextPos);
        pos = nextPos;
    }

    return trajectory;
}

void Tank::UpdatePosition(float terrainHeight)
{
    if (!IsAlive())
        return;

    const float TANK_HALF_HEIGHT = 18.0f;
    const float TANK_BASE_WIDTH = 25.0f;

    float leftX = position.x - TANK_BASE_WIDTH / 2;
    float rightX = position.x + TANK_BASE_WIDTH / 2;

    float leftHeight = terrain->GetHeightAtPosition(leftX);
    float rightHeight = terrain->GetHeightAtPosition(rightX);

    glm::vec2 terrainVector = glm::vec2(TANK_BASE_WIDTH, rightHeight - leftHeight);

    float newRotation = atan2(terrainVector.y, terrainVector.x);

    const float MAX_SLOPE_ANGLE = glm::radians(50.0f);
    rotation = glm::clamp(newRotation, -MAX_SLOPE_ANGLE, MAX_SLOPE_ANGLE);

    float avgHeight = (leftHeight + rightHeight) / 2.0f;
    float verticalOffset = TANK_HALF_HEIGHT * cos(rotation);

    position.y = avgHeight + verticalOffset;
}