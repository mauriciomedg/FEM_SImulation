#include "pch.h"
#include "SoftSimulEngine.h"

static float g_time = 0.0f;

extern "C"
{
    void InitializePlugin()
    {
        g_time = 0.0f;
    }

    int GetPluginVersion()
    {
        return 1;
    }

    float StepTest(float dt)
    {
        g_time += dt;
        return g_time;
    }
}