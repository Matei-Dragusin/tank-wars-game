#include "lab_m1/Tema1/projectile.h"
#include "core/engine.h"

using namespace m1;

Projectile::Projectile(const glm::vec2 &pos, const glm::vec2 &vel)
    : position(pos), velocity(vel), isActive(true)
{
}

void Projectile::Update(float deltaTime)
{
    if (!isActive || deltaTime <= 0.0f)
        return;

    velocity.y -= GRAVITY * deltaTime;
    position += velocity * deltaTime;


    if (position.y < 0 || position.x < 0 || position.x > 1280)
    {
        isActive = false;
    }
}