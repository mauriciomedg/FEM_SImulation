#pragma once

#ifdef SOFTSIMULENGINE_EXPORTS
#define SOFTSIMULENGINE_API __declspec(dllexport)
#else
#define SOFTSIMULENGINE_API __declspec(dllimport)
#endif

extern "C"
{
    SOFTSIMULENGINE_API void InitializePlugin();
    SOFTSIMULENGINE_API int GetPluginVersion();
    SOFTSIMULENGINE_API float StepTest(float dt);
    SOFTSIMULENGINE_API void SetVertices(const float* vertices, int vertexCount);
    SOFTSIMULENGINE_API void DeformVertices(float dt);
    SOFTSIMULENGINE_API void GetVertices(float* outVertices, int vertexCount);
}