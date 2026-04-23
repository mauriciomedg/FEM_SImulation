using System.Runtime.InteropServices;
using UnityEngine;

public class FEMPlugin
{
    [DllImport("SoftSimulEngine")]
    public static extern void InitializePlugin();

    [DllImport("SoftSimulEngine")]
    public static extern int GetPluginVersion();

    [DllImport("SoftSimulEngine")]
    public static extern float StepTest(float dt);
}
