#include "lab_m1/Tema1/Tema1.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
    // Clear all meshes
    for (auto &mesh : meshes)
    {
        if (mesh.second)
        {
            delete mesh.second;
            mesh.second = nullptr;
        }
    }
    meshes.clear();

    delete terrain;
    delete tank1;
    delete tank2;

    for (auto projectile : activeProjectiles)
    {
        delete projectile;
    }
    activeProjectiles.clear();
}

void Tema1::Init()
{
    // Setup game space
    logicalSpace = glm::vec2(1280, 720);
    resolution = window->GetResolution();

    // Setup camera
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->SetOrthographic(0, logicalSpace.x, 0, logicalSpace.y, 0.1f, 100);

    // Initialize terrain
    terrain = new Terrain(logicalSpace);
    AddMeshToList(terrain->CreateMesh("terrain"));

    // Create game meshes
    CreateGameMeshes();

    // Initialize tanks
    float tank1X = 200;
    float tank2X = logicalSpace.x - 200;
    tank1 = new Tank(glm::vec2(tank1X, terrain->GetHeightAtPosition(tank1X)), glm::vec3(1, 0, 0));
    tank2 = new Tank(glm::vec2(tank2X, terrain->GetHeightAtPosition(tank2X)), glm::vec3(0, 0, 1));
    tank1->SetTerrain(terrain);
    tank2->SetTerrain(terrain);

    // Initialize shake parameters
    shakeAmount = 0.0f;
    shakeDuration = 0.0f;

    timeOfDay = 0.0f;
    currentSkyColor = CalculateSkyColor();
}

void Tema1::UpdateTimeOfDay(float deltaTime)
{
    timeOfDay += deltaTime / DAY_LENGTH;
    if (timeOfDay > 1.0f)
    {
        timeOfDay = 0.0f;
    }
    currentSkyColor = CalculateSkyColor();
}

glm::vec3 Tema1::CalculateSkyColor() const
{
    // Dawn: 0.0-0.2, Day: 0.2-0.8, Dusk: 0.8-1.0
    glm::vec3 dawnColor = glm::vec3(0.97f, 0.65f, 0.45f); // Warm orange
    glm::vec3 dayColor = glm::vec3(0.4f, 0.7f, 1.0f);     // Bright blue
    glm::vec3 duskColor = glm::vec3(0.8f, 0.4f, 0.35f);   // Reddish orange
    glm::vec3 nightColor = glm::vec3(0.05f, 0.1f, 0.2f);  // Dark blue

    float t;
    glm::vec3 result;

    if (timeOfDay < 0.2f)
    { // Dawn
        t = timeOfDay / 0.2f;
        result = glm::mix(nightColor, dawnColor, t);
    }
    else if (timeOfDay < 0.3f)
    { // Dawn to day
        t = (timeOfDay - 0.2f) / 0.1f;
        result = glm::mix(dawnColor, dayColor, t);
    }
    else if (timeOfDay < 0.7f)
    { // Day
        result = dayColor;
    }
    else if (timeOfDay < 0.8f)
    { // Day to dusk
        t = (timeOfDay - 0.7f) / 0.1f;
        result = glm::mix(dayColor, duskColor, t);
    }
    else if (timeOfDay < 0.9f)
    { // Dusk to night
        t = (timeOfDay - 0.8f) / 0.1f;
        result = glm::mix(duskColor, nightColor, t);
    }
    else
    { // Night
        result = nightColor;
    }

    return result;
}

void Tema1::CreateGameMeshes()
{
    // Tank body mesh (two overlapping trapezoids)
    {
        vector<VertexFormat> vertices = {
            // Lower trapezoid (darker for shadow effect)
            VertexFormat(glm::vec3(-30, -20, 0), glm::vec3(0.4f, 0.4f, 0.4f)), // darker shadow
            VertexFormat(glm::vec3(30, -20, 0), glm::vec3(0.4f, 0.4f, 0.4f)),
            VertexFormat(glm::vec3(40, 0, 0), glm::vec3(0.5f, 0.5f, 0.5f)), // transition to medium
            VertexFormat(glm::vec3(-40, 0, 0), glm::vec3(0.5f, 0.5f, 0.5f)),

            // Upper trapezoid (lighter for highlight)
            VertexFormat(glm::vec3(-40, 0, 0), glm::vec3(0.6f, 0.6f, 0.6f)), // medium
            VertexFormat(glm::vec3(40, 0, 0), glm::vec3(0.6f, 0.6f, 0.6f)),
            VertexFormat(glm::vec3(30, 20, 0), glm::vec3(0.75f, 0.75f, 0.75f)), // highlight
            VertexFormat(glm::vec3(-30, 20, 0), glm::vec3(0.75f, 0.75f, 0.75f))};

        vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3,
            4, 5, 6,
            4, 6, 7};

        CreateMesh("tank_body", vertices, indices);
    }

    // Tank turret base (fixed circle with radial gradient)
    {
        vector<VertexFormat> vertices;
        vector<unsigned int> indices;

        const int numSegments = 32;
        const float radius = 15.0f;
        const float centerY = Tank::TURRET_CENTER_OFFSET;

        // Center of the circle (lighter)
        vertices.push_back(VertexFormat(glm::vec3(0, centerY, 0), glm::vec3(0.7f, 0.7f, 0.7f)));

        // Points on the circle (darker towards the edge for 3D effect)
        for (int i = 0; i <= numSegments; i++)
        {
            float theta = 2.0f * glm::pi<float>() * i / numSegments;
            float x = radius * cos(theta);
            float y = centerY + radius * sin(theta);
            vertices.push_back(VertexFormat(glm::vec3(x, y, 0), glm::vec3(0.5f, 0.5f, 0.5f)));
        }

        for (int i = 0; i < numSegments; i++)
        {
            indices.push_back(0);
            indices.push_back(i + 1);
            indices.push_back(i + 2);
        }

        CreateMesh("tank_turret_base", vertices, indices);
    }

    // Tank barrel (barrel with length gradient)
    {
        const float barrelWidth = 4.0f;
        const float barrelLength = Tank::BARREL_LENGTH;

        vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(-barrelWidth / 2, 0, 0), glm::vec3(0.7f)),           // bottom left
            VertexFormat(glm::vec3(barrelWidth / 2, 0, 0), glm::vec3(0.7f)),            // bottom right
            VertexFormat(glm::vec3(barrelWidth / 2, barrelLength, 0), glm::vec3(0.7f)), // top right
            VertexFormat(glm::vec3(-barrelWidth / 2, barrelLength, 0), glm::vec3(0.7f)) // top left
        };

        vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3};

        CreateMesh("tank_barrel", vertices, indices);
    }

    // Projectile mesh (disc with radial gradient)
    {
        vector<VertexFormat> vertices;
        vector<unsigned int> indices;

        // Center of the disc (lighter)
        vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), glm::vec3(1.0f, 1.0f, 1.0f)));

        const int numSegments = 20;
        const float radius = 5.0f;

        // Points on the circumference (darker)
        for (int i = 0; i <= numSegments; i++)
        {
            float theta = 2.0f * M_PI * i / numSegments;
            vertices.push_back(VertexFormat(
                glm::vec3(radius * cos(theta), radius * sin(theta), 0),
                glm::vec3(0.7f, 0.7f, 0.7f)));

            if (i < numSegments)
            {
                indices.push_back(0);
                indices.push_back(i + 1);
                indices.push_back(i + 2);
            }
        }

        CreateMesh("projectile", vertices, indices);
    }

    // Health bar mesh (keep green color but add gradient)
    {
        vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0, 0.7f, 0)),   // bottom left - darker green
            VertexFormat(glm::vec3(80, 0, 0), glm::vec3(0, 0.7f, 0)),  // bottom right
            VertexFormat(glm::vec3(80, 10, 0), glm::vec3(0, 1.0f, 0)), // top right - lighter green
            VertexFormat(glm::vec3(0, 10, 0), glm::vec3(0, 1.0f, 0))   // top left
        };

        vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3};

        Mesh *healthBar = new Mesh("health_bar");
        healthBar->InitFromData(vertices, indices);
        AddMeshToList(healthBar);
    }

    // Trajectory line mesh (keep as is)
    {
        Mesh *trajectoryMesh = new Mesh("trajectory_line");
        trajectoryMesh->SetDrawMode(GL_LINES);
        meshes[trajectoryMesh->GetMeshID()] = trajectoryMesh;
    }
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    if (deltaTime <= 0.0f)
        return;

    // Tank 1 controls (WASD + QE for power)
    if (tank1->IsAlive())
    {
        if (window->KeyHold(GLFW_KEY_A))
            tank1->Move(-1, deltaTime);
        if (window->KeyHold(GLFW_KEY_D))
            tank1->Move(1, deltaTime);
        if (window->KeyHold(GLFW_KEY_W))
            tank1->RotateTurret(1, deltaTime);
        if (window->KeyHold(GLFW_KEY_S))
            tank1->RotateTurret(-1, deltaTime);
        if (window->KeyHold(GLFW_KEY_Q))
            tank1->IncreasePower();
        if (window->KeyHold(GLFW_KEY_E))
            tank1->DecreasePower();
    }

    // Tank 2 controls (Arrows + OP for power)
    if (tank2->IsAlive())
    {
        if (window->KeyHold(GLFW_KEY_LEFT))
            tank2->Move(-1, deltaTime);
        if (window->KeyHold(GLFW_KEY_RIGHT))
            tank2->Move(1, deltaTime);
        if (window->KeyHold(GLFW_KEY_UP))
            tank2->RotateTurret(1, deltaTime);
        if (window->KeyHold(GLFW_KEY_DOWN))
            tank2->RotateTurret(-1, deltaTime);
        if (window->KeyHold(GLFW_KEY_O))
            tank2->IncreasePower();
        if (window->KeyHold(GLFW_KEY_P))
            tank2->DecreasePower();
    }
}

void Tema1::OnKeyPress(int key, int mods)
{
    // Tank 1 fire
    if (key == GLFW_KEY_SPACE && tank1 && tank1->IsAlive())
    {
        Projectile *proj = tank1->Fire();
        if (proj)
        {
            activeProjectiles.push_back(proj);
        }
    }

    // Tank 2 fire
    if (key == GLFW_KEY_ENTER && tank2 && tank2->IsAlive())
    {
        Projectile *proj = tank2->Fire();
        if (proj)
        {
            activeProjectiles.push_back(proj);
        }
    }
}

void Tema1::Update(float deltaTimeSeconds)
{
    UpdateTimeOfDay(deltaTimeSeconds);
    UpdateCameraShake(deltaTimeSeconds);
    if (deltaTimeSeconds <= 0.0f)
        return;

    UpdateTanks(deltaTimeSeconds);
    UpdateProjectiles(deltaTimeSeconds);
    terrain->UpdateLandslide(deltaTimeSeconds);
    CheckCollisions();
    RenderScene();
}

void Tema1::UpdateTanks(float deltaTime)
{
    if (!tank1 || !tank2)
        return;

    if (tank1->IsAlive())
    {
        float currentHeight = terrain->GetHeightAtPosition(tank1->GetPosition().x);
        tank1->UpdatePosition(currentHeight);
    }

    if (tank2->IsAlive())
    {
        float currentHeight = terrain->GetHeightAtPosition(tank2->GetPosition().x);
        tank2->UpdatePosition(currentHeight);
    }
}

void Tema1::UpdateProjectiles(float deltaTime)
{
    auto it = activeProjectiles.begin();
    while (it != activeProjectiles.end())
    {
        if (!(*it))
        {
            it = activeProjectiles.erase(it);
            continue;
        }

        (*it)->Update(deltaTime);

        if (!(*it)->IsActive())
        {
            delete *it;
            it = activeProjectiles.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Tema1::CheckCollisions()
{
    for (auto projectile : activeProjectiles)
    {
        if (!projectile || !projectile->IsActive())
            continue;

        // Terrain collision
        float terrainHeight = terrain->GetHeightAtPosition(projectile->GetPosition().x);
        if (projectile->GetPosition().y <= terrainHeight)
        {
            TriggerCameraShake(15.0f, 0.3f);
            terrain->DeformTerrainAtPoint(projectile->GetPosition().x, 30.0f, 20.0f);
            projectile->Deactivate();
            continue;
        }

        // Tank collision
        const float TANK_RADIUS = 30.0f;

        if (tank1->IsAlive())
        {
            float dist1 = glm::distance(projectile->GetPosition(), tank1->GetPosition());
            if (dist1 < TANK_RADIUS)
            {
                TriggerCameraShake(20.0f, 0.5f);
                tank1->TakeDamage(25.0f);
                projectile->Deactivate();
                continue;
            }
        }

        if (tank2->IsAlive())
        {
            float dist2 = glm::distance(projectile->GetPosition(), tank2->GetPosition());
            if (dist2 < TANK_RADIUS)
            {
                TriggerCameraShake(20.0f, 0.5f);
                tank2->TakeDamage(25.0f);
                projectile->Deactivate();
                continue;
            }
        }
    }
}

void Tema1::RenderScene()
{
    // Render terrain
    RenderMesh(meshes["terrain"], shaders["VertexColor"], glm::mat4(1));

    // Render tanks
    if (tank1->IsAlive())
    {
        tank1->RenderTrajectory(this);
        tank1->RenderTank(this);
    }

    if (tank2->IsAlive())
    {
        tank2->RenderTrajectory(this);
        tank2->RenderTank(this);
    }

    // Render projectiles
    for (const auto &projectile : activeProjectiles)
    {
        if (!projectile || !projectile->IsActive())
            continue;

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(projectile->GetPosition(), 0));
        RenderMesh(meshes["projectile"], shaders["VertexColor"], modelMatrix);
    }
}

void Tema1::TriggerCameraShake(float intensity, float duration)
{
    shakeAmount = intensity;
    shakeDuration = duration;
}

void Tema1::UpdateCameraShake(float deltaTime)
{
    if (shakeDuration > 0)
    {
        // Calculate random offset for the camera
        float offsetX = (rand() % 100 - 50) * 0.01f * shakeAmount;
        float offsetY = (rand() % 100 - 50) * 0.01f * shakeAmount;

        // Calculate the base position of the camera
        const glm::vec3 basePosition(0, 0, 50);

        // Apply the shake while keeping the Z coordinate unchanged
        GetSceneCamera()->SetPosition(
            glm::vec3(basePosition.x + offsetX,
                      basePosition.y + offsetY,
                      basePosition.z));

        // Reduce the duration and intensity
        shakeDuration -= deltaTime;
        shakeAmount *= 0.9f;

        // Reset the camera when done
        if (shakeDuration <= 0)
        {
            GetSceneCamera()->SetPosition(basePosition);
            shakeAmount = 0;
        }
    }
}

void Tema1::FrameStart()
{
    glClearColor(currentSkyColor.r, currentSkyColor.g, currentSkyColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::OnWindowResize(int width, int height)
{
    resolution = glm::ivec2(width, height);
    glViewport(0, 0, width, height);
    GetSceneCamera()->SetOrthographic(0, logicalSpace.x, 0, logicalSpace.y, 1.0f, 100);
}

void Tema1::CreateMesh(const std::string &name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices)
{
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    unsigned int IBO = 0;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void *)(sizeof(glm::vec3)));

    // Set vertex normal attribute (if used)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void *)(2 * sizeof(glm::vec3)));

    // Set vertex texture coordinate attribute (if used)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void *)(3 * sizeof(glm::vec3)));

    Mesh *mesh = new Mesh(name);
    mesh->InitFromData(vertices, indices);
    AddMeshToList(mesh);

    // Cleanup
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    glDeleteVertexArrays(1, &VAO);
}