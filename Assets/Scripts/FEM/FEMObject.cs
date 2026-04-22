using UnityEngine;

public class FEMObject : MonoBehaviour
{
    [SerializeField] private Transform visualMesh;
    [SerializeField] private Transform mechanicalMeshDebug;
    [SerializeField] private Transform constraintHandles;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        Debug.Log($"FEMObject ready: {name}");
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
