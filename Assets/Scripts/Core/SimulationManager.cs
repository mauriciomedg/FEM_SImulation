using UnityEngine;

public class SimulationManager : MonoBehaviour
{
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        FEMPlugin.InitializePlugin();
        int version = FEMPlugin.GetPluginVersion();
        Debug.Log($"SimulationManager initialized. FEM plugin version: {version}");
    }

    // Update is called once per frame
    void Update()
    {
        float value = FEMPlugin.StepTest(Time.deltaTime);
        Debug.Log($"Native test value: {value}");
    }
}
