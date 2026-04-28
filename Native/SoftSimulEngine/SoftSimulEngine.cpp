#include "pch.h"
#include "SoftSimulEngine.h"
#include <vector>
#include <cmath>
#include <algorithm>

struct Vec3
{
    float x, y, z;
};

struct Tet
{
    int i0, i1, i2, i3;
};

static std::vector<Vec3> g_restNodes;
static std::vector<Vec3> g_nodes;
static std::vector<Tet> g_tets;
static float g_time = 0.0f;

static std::vector<float> g_originalVertices;
static std::vector<float> g_deformedVertices;


extern "C"
{
    void InitializePlugin()
    {
        g_time = 0.0f;
        g_originalVertices.clear();
        g_deformedVertices.clear();
    }

    int GetPluginVersion()
    {
        return 1;
    }

    void CreateTestTet()
    {
        g_restNodes.clear();
        g_nodes.clear();
        g_tets.clear();
        g_time = 0.0f;

        // One tetrahedron
        g_restNodes.push_back({ 0.0f, 0.0f, 0.0f }); // 0
        g_restNodes.push_back({ 1.0f, 0.0f, 0.0f }); // 1
        g_restNodes.push_back({ 0.5f, 0.0f, 1.0f }); // 2
        g_restNodes.push_back({ 0.5f, 1.0f, 0.5f }); // 3 top

        g_nodes = g_restNodes;

        g_tets.push_back({ 0, 1, 2, 3 });
    }

    void StepSimulation(float dt)
    {
        g_time += dt;

        if (g_nodes.size() != g_restNodes.size())
            return;

        // Fake deformation:
        // Keep base nodes fixed, move the top node up/down a bit.
        for (size_t i = 0; i < g_nodes.size(); ++i)
        {
            g_nodes[i] = g_restNodes[i];
        }

        if (g_nodes.size() >= 4)
        {
            g_nodes[3].y = g_restNodes[3].y + 0.25f * std::sinf(g_time * 2.0f);
        }
    }

    int GetNodeCount()
    {
        return static_cast<int>(g_nodes.size());
    }

    void GetNodePositions(float* outPositions, int nodeCount)
    {
        if (outPositions == nullptr || nodeCount <= 0)
            return;

        const int availableNodes = static_cast<int>(g_nodes.size());
        const int count = min(nodeCount, availableNodes);

        for (int i = 0; i < count; ++i)
        {
            outPositions[i * 3 + 0] = g_nodes[i].x;
            outPositions[i * 3 + 1] = g_nodes[i].y;
            outPositions[i * 3 + 2] = g_nodes[i].z;
        }
    }

    int GetTetCount()
    {
        return static_cast<int>(g_tets.size());
    }

    void GetTetIndices(int* outIndices, int tetCount)
    {
        if (outIndices == nullptr || tetCount <= 0)
            return;

        const int availableTets = static_cast<int>(g_tets.size());
        const int count = min(tetCount, availableTets);

        for (int i = 0; i < count; ++i)
        {
            outIndices[i * 4 + 0] = g_tets[i].i0;
            outIndices[i * 4 + 1] = g_tets[i].i1;
            outIndices[i * 4 + 2] = g_tets[i].i2;
            outIndices[i * 4 + 3] = g_tets[i].i3;
        }
    }

    void SetVertices(const float* vertices, int vertexCount)
    {
        if (vertices == nullptr || vertexCount <= 0)
            return;

        const int floatCount = vertexCount * 3;

        g_originalVertices.assign(vertices, vertices + floatCount);
        g_deformedVertices = g_originalVertices;
    }

    void DeformVertices(float dt)
    {
        g_time += dt;

        if (g_originalVertices.empty())
            return;

        const int vertexCount = static_cast<int>(g_originalVertices.size() / 3);

        for (int i = 0; i < vertexCount; ++i)
        {
            const int baseIndex = i * 3;

            const float x = g_originalVertices[baseIndex + 0];
            const float y = g_originalVertices[baseIndex + 1];
            const float z = g_originalVertices[baseIndex + 2];

            const float offset = 0.1f * std::sinf(g_time * 2.0f + x + z);

            g_deformedVertices[baseIndex + 0] = x;
            g_deformedVertices[baseIndex + 1] = y + offset;
            g_deformedVertices[baseIndex + 2] = z;
        }
    }

    void GetVertices(float* outVertices, int vertexCount)
    {
        if (outVertices == nullptr || vertexCount <= 0)
            return;

        const int floatCount = vertexCount * 3;
        const int availableFloatCount = static_cast<int>(g_deformedVertices.size());

        const int copyCount = floatCount < availableFloatCount ? floatCount : availableFloatCount; //std::min(floatCount, availableFloatCount);

        for (int i = 0; i < copyCount; ++i)
        {
            outVertices[i] = g_deformedVertices[i];
        }
    }

    float StepTest(float dt)
    {
        g_time += dt;
        return g_time;
    }

}