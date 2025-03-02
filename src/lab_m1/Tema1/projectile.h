// projectile.h
#pragma once
#include "utils/glm_utils.h"

namespace m1
{
    class Projectile
    {
    public:
        Projectile(const glm::vec2 &position, const glm::vec2 &velocity);
        ~Projectile() = default;

        void Update(float deltaTime);
        bool IsActive() const { return isActive; }
        void Deactivate() { isActive = false; }
        glm::vec2 GetPosition() const { return position; }

    private:
        glm::vec2 position;
        glm::vec2 velocity;
        bool isActive;

        static constexpr float GRAVITY = 981.0f;
    };
}