using UnityEngine;

public class FEMObject : MonoBehaviour
{
    [SerializeField] private Transform visualMesh;
    [SerializeField] private Transform mechanicalMeshDebug;
    [SerializeField] private Transform constraintHandles;

    private Mesh _mesh;
    private Vector3[] _originalVertices;
    private Vector3[] _deformedVertices;
    private float[] _vertexBuffer;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        Debug.Log($"FEMObject ready: {name}");

        if (visualMesh == null)
        {
            Debug.LogError("VisualMesh reference is missing.");
            return;
        }

        MeshFilter meshFilter = visualMesh.GetComponentInChildren<MeshFilter>();
        if (meshFilter == null)
        {
            Debug.LogError("No MeshFilter found under VisualMesh.");
            return;
        }

        _mesh = meshFilter.mesh;
        _originalVertices = _mesh.vertices;
        _deformedVertices = new Vector3[_originalVertices.Length];
        _vertexBuffer = new float[_originalVertices.Length * 3];

        for (int i = 0; i < _originalVertices.Length; ++i)
        {
            _deformedVertices[i] = _originalVertices[i];
            _vertexBuffer[i * 3 + 0] = _originalVertices[i].x;
            _vertexBuffer[i * 3 + 1] = _originalVertices[i].y;
            _vertexBuffer[i * 3 + 2] = _originalVertices[i].z;
        }

        FEMPlugin.InitializePlugin();
        FEMPlugin.SetVertices(_vertexBuffer, _originalVertices.Length);
    }

    // Update is called once per frame
    void Update()
    {
        if (_mesh == null)
            return;

        FEMPlugin.DeformVertices(Time.deltaTime);
        FEMPlugin.GetVertices(_vertexBuffer, _originalVertices.Length);

        for (int i = 0; i < _deformedVertices.Length; ++i)
        {
            _deformedVertices[i].x = _vertexBuffer[i * 3 + 0];
            _deformedVertices[i].y = _vertexBuffer[i * 3 + 1];
            _deformedVertices[i].z = _vertexBuffer[i * 3 + 2];
        }

        _mesh.vertices = _deformedVertices;
        _mesh.RecalculateNormals();
        _mesh.RecalculateBounds();
    }
}
