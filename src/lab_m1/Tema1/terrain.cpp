#include "lab_m1/Tema1/terrain.h"
#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace m1;

Terrain::Terrain(const glm::vec2 &logicalSpace, float pointDensity)
    : logicalSpace(logicalSpace), pointDensity(pointDensity), terrainMesh(nullptr)
{
    GenerateTerrainPoints();
}

Terrain::~Terrain()
{
    if (terrainMesh)
    {
        delete terrainMesh;
    }
}

float Terrain::TerrainFunction(float x) const
{
    const float baseHeight = logicalSpace.y * 0.3f;
    const float amplitude1 = 80.0f;
    const float amplitude2 = 40.0f;
    const float amplitude3 = 20.0f;
    const float frequency1 = 0.002f;
    const float frequency2 = 0.005f;
    const float frequency3 = 0.01f;

    float wave1 = amplitude1 * sin(frequency1 * x);
    float wave2 = amplitude2 * sin(frequency2 * x * 2.0f);
    float wave3 = amplitude3 * sin(frequency3 * x * 3.0f);

    return baseHeight + wave1 + wave2 + wave3;
}

void Terrain::GenerateTerrainPoints()
{
    terrainPoints.clear(); // Clear the vector for regeneration

    // Calculate number of points needed based on screen width and density
    // Example: if width = 1280 and density = 10, we get 129 points
    int numPoints = static_cast<int>(logicalSpace.x / pointDensity) + 1;

    // Generate terrain points by sampling the height function
    for (int i = 0; i < numPoints; i++)
    {
        float x = i * pointDensity;   // Points placed at regular intervals
        float y = TerrainFunction(x); // Calculate height at this position
        terrainPoints.push_back(glm::vec2(x, y));
    }

    // Generate indices for rendering triangles
    indices.clear();
    for (size_t i = 0; i < terrainPoints.size() - 1; i++)
    {
        // For each terrain segment, we use 4 vertices to create a rectangle
        // from terrain height down to ground level
        size_t baseIndex = i * 4;

        // First triangle: top-left, top-right, bottom-right
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);

        // Second triangle: top-left, bottom-right, bottom-left
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
    }
}

void Terrain::GenerateMeshData(std::vector<VertexFormat> &vertices)
{
    vertices.clear();

    // Colors for the terrain
    const glm::vec3 darkBrown = glm::vec3(0.3f, 0.2f, 0.1f);
    const glm::vec3 mediumBrown = glm::vec3(0.5f, 0.35f, 0.2f);
    const glm::vec3 lightBrown = glm::vec3(0.7f, 0.5f, 0.3f);
    const glm::vec3 bottomBrown = glm::vec3(0.25f, 0.15f, 0.1f);

    // Max and min height for the terrain
    float minHeight = terrainPoints[0].y;
    float maxHeight = terrainPoints[0].y;
    for (const auto &point : terrainPoints)
    {
        minHeight = std::min(minHeight, point.y);
        maxHeight = std::max(maxHeight, point.y);
    }
    float heightRange = maxHeight - minHeight;

    // Generate vertices for the terrain
    for (size_t i = 0; i < terrainPoints.size() - 1; i++)
    {
        glm::vec2 p1 = terrainPoints[i];
        glm::vec2 p2 = terrainPoints[i + 1];

        // Calculate the height factors for the two points
        float heightFactor1 = (p1.y - minHeight) / heightRange;
        float heightFactor2 = (p2.y - minHeight) / heightRange;

        // Calculate the colors for the two points
        glm::vec3 color1, color2;
        if (heightFactor1 < 0.5f)
        {
            float t = heightFactor1 * 2.0f;
            color1 = glm::mix(darkBrown, mediumBrown, t);
        }
        else
        {
            float t = (heightFactor1 - 0.5f) * 2.0f;
            color1 = glm::mix(mediumBrown, lightBrown, t);
        }

        if (heightFactor2 < 0.5f)
        {
            float t = heightFactor2 * 2.0f;
            color2 = glm::mix(darkBrown, mediumBrown, t);
        }
        else
        {
            float t = (heightFactor2 - 0.5f) * 2.0f;
            color2 = glm::mix(mediumBrown, lightBrown, t);
        }

        // Adăugăm vertex-urile pentru acest segment
        vertices.push_back(VertexFormat(glm::vec3(p1.x, p1.y, 0), color1));
        vertices.push_back(VertexFormat(glm::vec3(p2.x, p2.y, 0), color2));
        vertices.push_back(VertexFormat(glm::vec3(p2.x, 0, 0), bottomBrown));
        vertices.push_back(VertexFormat(glm::vec3(p1.x, 0, 0), bottomBrown));
    }
}

Mesh *Terrain::CreateMesh(const std::string &name)
{
    if (terrainMesh)
    {
        delete terrainMesh;
    }

    // Generate vertices for the terrain
    GenerateMeshData(vertices);

    // Creăm noul mesh
    terrainMesh = new Mesh(name);
    terrainMesh->InitFromData(vertices, indices);
    return terrainMesh;
}

float Terrain::GetHeightAtPosition(float x) const
{
    // Find the segment that contains the given x
    for (size_t i = 0; i < terrainPoints.size() - 1; i++)
    {
        if (x >= terrainPoints[i].x && x <= terrainPoints[i + 1].x)
        {
            // Calculate the interpolation factor
            float t = (x - terrainPoints[i].x) / (terrainPoints[i + 1].x - terrainPoints[i].x);

            // Get the two points that define the segment
            glm::vec2 A = terrainPoints[i];
            glm::vec2 B = terrainPoints[i + 1];

            // Interpolate the height
            return A.y * (1.0f - t) + B.y * t;
        }
    }

    // If the x is outside the terrain, return the height of the closest point
    if (x < terrainPoints.front().x)
        return terrainPoints.front().y;
    return terrainPoints.back().y;
}

void Terrain::DeformTerrainAtPoint(float x, float radius, float strength)
{
    float maxDeformation = 0.0f;

    for (auto &point : terrainPoints)
    {
        float distance = std::abs(point.x - x);
        if (distance < radius)
        {
            float deformFactor = cos((distance / radius) * glm::pi<float>() * 0.5f);
            float deformation = strength * deformFactor;
            point.y -= deformation;
            maxDeformation = std::max(maxDeformation, deformation);
        }
    }

    if (maxDeformation > 0.0f)
    {
        // Regenerate the mesh data using the helper function
        GenerateMeshData(vertices);

        // Update the existing mesh
        if (terrainMesh)
        {
            terrainMesh->InitFromData(vertices, indices);
        }
    }
}

void Terrain::UpdateLandslide(float deltaTime)
{
    bool hasChanged = false;

    for (size_t i = 0; i < terrainPoints.size() - 1; i++)
    {
        float dx = terrainPoints[i + 1].x - terrainPoints[i].x;
        float dy = terrainPoints[i + 1].y - terrainPoints[i].y;
        float angle = std::abs(glm::degrees(std::atan2(dy, dx)));

        if (angle > LANDSLIDE_THRESHOLD)
        {
            float transfer = LANDSLIDE_TRANSFER * deltaTime;
            if (terrainPoints[i + 1].y > terrainPoints[i].y)
            {
                float amount = std::min(transfer, (terrainPoints[i + 1].y - terrainPoints[i].y) * 0.5f);
                terrainPoints[i + 1].y -= amount;
                terrainPoints[i].y += amount;
            }
            else
            {
                float amount = std::min(transfer, (terrainPoints[i].y - terrainPoints[i + 1].y) * 0.5f);
                terrainPoints[i + 1].y += amount;
                terrainPoints[i].y -= amount;
            }
            hasChanged = true;
        }
    }

    if (hasChanged)
    {
        GenerateMeshData(vertices);
        if (terrainMesh)
        {
            terrainMesh->InitFromData(vertices, indices);
        }
    }
}