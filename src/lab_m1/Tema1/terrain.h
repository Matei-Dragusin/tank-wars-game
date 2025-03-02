#pragma once

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"
#include <vector>

namespace m1 
{
    class Terrain 
    {
    public:
        Terrain(const glm::vec2& logicalSpace, float pointDensity = 10.f);
        ~Terrain();

        Mesh* CreateMesh(const std::string& name);
        void DeformTerrainAtPoint(float x, float radius, float strength);
        void UpdateLandslide(float deltaTime);
        float GetHeightAtPosition(float x) const;
        const std::vector<glm::vec2>& GetTerrainPoints() const { return terrainPoints; }
        
    private:
        void GenerateMeshData(std::vector<VertexFormat>& vertices);
        float TerrainFunction(float x) const;
        void GenerateTerrainPoints();
        
        std::vector<glm::vec2> terrainPoints;
        glm::vec2 logicalSpace;
        float pointDensity;
        
        const float LANDSLIDE_THRESHOLD = 45.0f;
        const float LANDSLIDE_TRANSFER = 5.0f;
        
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;
        Mesh* terrainMesh;
    };
}