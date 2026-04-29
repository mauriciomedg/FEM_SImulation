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


    SOFTSIMULENGINE_API void CreateTestTet();
    SOFTSIMULENGINE_API void StepSimulation(float dt);
    SOFTSIMULENGINE_API int GetNodeCount();
    SOFTSIMULENGINE_API void GetNodePositions(float* outPositions, int nodeCount);
    SOFTSIMULENGINE_API int GetTetCount();
    SOFTSIMULENGINE_API void GetTetIndices(int* outIndices, int tetCount);
    SOFTSIMULENGINE_API void AddImpulseToNode(int nodeIndex, float ix, float iy, float iz);

}