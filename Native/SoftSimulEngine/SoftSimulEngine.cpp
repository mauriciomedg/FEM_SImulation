#include "pch.h"
#include "SoftSimulEngine.h"
#include <vector>
#include <cmath>

static float g_time = 0.0f;
static std::vector<float> g_vertices;


extern "C"
{
    void InitializePlugin()
    {
        g_time = 0.0f;
        g_vertices.clear();
    }

    int GetPluginVersion()
    {
        return 1;
    }

    void SetVertices(const float* vertices, int vertexCount)
    {
        g_vertices.assign(vertices, vertices + vertexCount * 3);
    }

    void DeformVertices(float dt)
    {
        g_time += dt;

        for (int i = 0; i < (int)g_vertices.size() / 3; ++i)
        {
            float x = g_vertices[i * 3 + 0];
            float baseY = g_vertices[i * 3 + 1];

            g_vertices[i * 3 + 1] = baseY + 0.1f * std::sin(g_time + x);
        }
    }

    void GetVertices(float* outVertices, int vertexCount)
    {
        int floatCount = vertexCount * 3;
        for (int i = 0; i < floatCount; ++i)
        {
            outVertices[i] = g_vertices[i];
        }
    }

    float StepTest(float dt)
    {
        g_time += dt;
        return g_time;
    }

}