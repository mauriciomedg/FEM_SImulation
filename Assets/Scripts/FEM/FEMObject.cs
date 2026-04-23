using UnityEngine;

public class FEMObject : MonoBehaviour
{
    [SerializeField] private Transform visualMesh;
    [SerializeField] private Transform mechanicalMeshDebug;
    [SerializeField] private Transform constraintHandles;

    private Vector3 _initialScale;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        Debug.Log($"FEMObject ready: {name}");

        if (visualMesh != null)
        {
            _initialScale = visualMesh.localScale;
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (visualMesh == null)
            return;

        float s = 1.0f + 0.1f * Mathf.Sin(Time.time * 2.0f);
        visualMesh.localScale = new Vector3(_initialScale.x, _initialScale.y * s, _initialScale.z);
    }
}
