// tank.h
#pragma once
#include "utils/glm_utils.h"
#include <vector>

namespace m1 
{
    class Terrain;
    class Projectile;

    class Tank 
    {
    public:
        Tank(const glm::vec2& startPosition, const glm::vec3& tankColor);
        ~Tank() = default;

        // Constants
        static constexpr float MAX_HEALTH = 100.0f;
        static constexpr float MIN_TURRET_ANGLE = -90.0f;  
        static constexpr float MAX_TURRET_ANGLE = 90.0f;   
        static constexpr float FIRE_COOLDOWN = 0.1f;
        static constexpr float BASE_PROJECTILE_SPEED = 500.0f;
        static constexpr float MIN_POWER = 0.5f;
        static constexpr float MAX_POWER = 2.0f;
        static constexpr float POWER_STEP = 0.01f;
        static constexpr float BARREL_LENGTH = 25.0f;
        static constexpr float TURRET_ROTATION_SPEED = 90.0f;
        static constexpr float TURRET_CENTER_OFFSET = 25.0f;
        static constexpr float MAX_MOVE_SPEED = 200.0f;

        // Movement and control
        void Move(float direction, float deltaTime);
        void RotateTurret(float direction, float deltaTime);
        void IncreasePower();
        void DecreasePower();
        Projectile* Fire();
        void SetTerrain(Terrain* t) { terrain = t; }
        
        // Position and state updates
        void UpdatePosition(float terrainHeight);
        void TakeDamage(float damage);
        std::vector<glm::vec2> CalculateTrajectory() const;
        void RenderTank(class Tema1* game);
        void RenderTrajectory(class Tema1* game);

        // Getters
        glm::vec2 GetPosition() const { return position; }
        glm::vec3 GetColor() const { return color; }
        bool IsAlive() const { return health > 0; }
        glm::vec2 GetBarrelTipPosition() const;
        glm::vec2 GetFiringDirection() const;
        bool CanFire() const;

    private:
        glm::vec2 position;
        glm::vec3 color;
        float rotation;          
        float turretRotation;    
        float health;
        float lastFireTime;
        float power;
        Terrain* terrain;
    };
}