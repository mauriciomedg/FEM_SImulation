#include "pch.h"
#include "SoftSimulEngine.h"
#include <vector>
#include <cmath>
#include <algorithm>

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