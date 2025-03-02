#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema1/tank.h"
#include "lab_m1/Tema1/projectile.h"
#include "lab_m1/Tema1/terrain.h"
#include <vector>

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
        friend class Tank;

    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnWindowResize(int width, int height) override;

        void CreateMesh(const std::string &name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices);
        void UpdateCameraShake(float deltaTime);
        void TriggerCameraShake(float intensity = 15.0f, float duration = 0.3f);
        void UpdateTimeOfDay(float deltaTime);
        glm::vec3 CalculateSkyColor() const;

        // Game objects
        Tank *tank1;
        Tank *tank2;
        Terrain *terrain;
        std::vector<Projectile *> activeProjectiles;

        glm::ivec2 resolution;
        glm::vec2 logicalSpace;

        // Camera shake
        float shakeAmount;
        float shakeDuration;

        // Time of day
        float timeOfDay;
        static constexpr float DAY_LENGTH = 360.0f;
        glm::vec3 currentSkyColor;

    protected:
        void CreateGameMeshes();
        void UpdateTanks(float deltaTime);
        void UpdateProjectiles(float deltaTime);
        void CheckCollisions();
        void RenderScene();
    };
}