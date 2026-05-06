using System.Runtime.InteropServices;
using UnityEngine;

public class FEMPlugin
{
    [DllImport("SoftSimulEngine")]
    public static extern void InitializePlugin();

    [DllImport("SoftSimulEngine")]
    public static extern int GetPluginVersion();

    [DllImport("SoftSimulEngine")]
    public static extern void CreateTestTet();

    [DllImport("SoftSimulEngine")]
    public static extern void StepSimulation(float dt);

    [DllImport("SoftSimulEngine")]
    public static extern int GetNodeCount();

    [DllImport("SoftSimulEngine")]
    public static extern void GetNodePositions(float[] outPositions, int nodeCount);

    [DllImport("SoftSimulEngine")]
    public static extern int GetTetCount();

    [DllImport("SoftSimulEngine")]
    public static extern void GetTetIndices(int[] outIndices, int tetCount);

    [DllImport("SoftSimulEngine")]
    public static extern void SetVertices(float[] vertices, int vertexCount);

    [DllImport("SoftSimulEngine")]
    public static extern void DeformVertices(float dt);

    [DllImport("SoftSimulEngine")]
    public static extern void GetVertices(float[] outVertices, int vertexCount);

    [DllImport("SoftSimulEngine")]
    public static extern float StepTest(float dt);

    [DllImport("SoftSimulEngine")]
    public static extern void AddImpulseToNode(int nodeIndex, float ix, float iy, float iz);

    [DllImport("SoftSimulEngine")]
    public static extern void GetFixedNodeFlags(int[] outFlags, int nodeCount);

    [DllImport("SoftSimulEngine")]
    public static extern int GetEdgeCount();

    [DllImport("SoftSimulEngine")]
    public static extern void GetEdges(int[] outEdges, int edgeCount);
}
