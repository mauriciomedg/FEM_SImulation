using UnityEngine;
using UnityEngine.InputSystem;

public class FEMObject : MonoBehaviour
{
    [SerializeField] private Transform visualMesh;
    [SerializeField] private Transform mechanicalMeshDebug;
    [SerializeField] private Transform constraintHandles;

    private float[] _nodePositionBuffer;
    private int[] _tetIndexBuffer;
    private int[] _fixedNodeFlags;

    private Vector3[] _nodePositions;
    private GameObject[] _nodeDebugObjects;

    private bool _initialized;

    //private Mesh _mesh;
    //private Vector3[] _originalVertices;
    //private Vector3[] _deformedVertices;
    //private float[] _vertexBuffer;

    private void Start()
    {
        Debug.Log($"FEMObject ready: {name}");

        FEMPlugin.InitializePlugin();
        FEMPlugin.CreateTestTet();

        int nodeCount = FEMPlugin.GetNodeCount();
        int tetCount = FEMPlugin.GetTetCount();

        Debug.Log($"Native tet object created. Nodes: {nodeCount}, Tets: {tetCount}");

        if (nodeCount <= 0 || tetCount <= 0)
        {
            Debug.LogError("FEMObject: Native test tet data is invalid.");
            return;
        }

        _nodePositionBuffer = new float[nodeCount * 3];
        _tetIndexBuffer = new int[tetCount * 4];
        _fixedNodeFlags = new int[nodeCount];
        _nodePositions = new Vector3[nodeCount];
        _nodeDebugObjects = new GameObject[nodeCount];

        FEMPlugin.GetNodePositions(_nodePositionBuffer, nodeCount);
        FEMPlugin.GetTetIndices(_tetIndexBuffer, tetCount);
        FEMPlugin.GetFixedNodeFlags(_fixedNodeFlags, nodeCount);

        CreateNodeDebugObjects();
        UpdateNodePositionsFromBuffer();
        ApplyNodePositionsToDebugObjects();

        _initialized = true;
    }

    private void Update()
    {
        if (!_initialized)
            return;

        if (Keyboard.current != null && Keyboard.current.spaceKey.wasPressedThisFrame)
        {
            FEMPlugin.AddImpulseToNode(3, 0.0f, 9.0f, 0.0f);
            Debug.Log("Impulse applied to top node.");
        }

        int nodeCount = _nodePositions.Length;
        int tetCount = _tetIndexBuffer.Length / 4;

        FEMPlugin.StepSimulation(Time.deltaTime);
        FEMPlugin.GetNodePositions(_nodePositionBuffer, nodeCount);

        UpdateNodePositionsFromBuffer();
        ApplyNodePositionsToDebugObjects();
        DrawTetEdges(tetCount);
    }

    private void CreateNodeDebugObjects()
    {
        if (mechanicalMeshDebug == null)
        {
            Debug.LogError("FEMObject: MechanicalMesh_Debug reference is missing.");
            return;
        }

        for (int i = 0; i < _nodeDebugObjects.Length; ++i)
        {
            GameObject sphere = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            sphere.name = $"Node_{i}";
            sphere.transform.SetParent(mechanicalMeshDebug, false);
            sphere.transform.localScale = Vector3.one * 0.08f;

            Collider collider = sphere.GetComponent<Collider>();
            if (collider != null)
            {
                Destroy(collider);
            }

            Renderer renderer = sphere.GetComponent<Renderer>();
            if (renderer != null)
            {
                bool isFixed = _fixedNodeFlags != null && i < _fixedNodeFlags.Length && _fixedNodeFlags[i] == 1;
                renderer.material.color = isFixed ? Color.red : Color.green;
            }

            _nodeDebugObjects[i] = sphere;
        }
    }

    private void UpdateNodePositionsFromBuffer()
    {
        for (int i = 0; i < _nodePositions.Length; ++i)
        {
            _nodePositions[i] = new Vector3(
                _nodePositionBuffer[i * 3 + 0],
                _nodePositionBuffer[i * 3 + 1],
                _nodePositionBuffer[i * 3 + 2]
            );
        }
    }

    private void ApplyNodePositionsToDebugObjects()
    {
        for (int i = 0; i < _nodeDebugObjects.Length; ++i)
        {
            if (_nodeDebugObjects[i] != null)
            {
                _nodeDebugObjects[i].transform.localPosition = _nodePositions[i];
            }
        }
    }

    private void DrawTetEdges(int tetCount)
    {
        for (int t = 0; t < tetCount; ++t)
        {
            int i0 = _tetIndexBuffer[t * 4 + 0];
            int i1 = _tetIndexBuffer[t * 4 + 1];
            int i2 = _tetIndexBuffer[t * 4 + 2];
            int i3 = _tetIndexBuffer[t * 4 + 3];

            DrawEdge(i0, i1);
            DrawEdge(i0, i2);
            DrawEdge(i0, i3);
            DrawEdge(i1, i2);
            DrawEdge(i1, i3);
            DrawEdge(i2, i3);
        }
    }

    private void DrawEdge(int a, int b)
    {
        if (a < 0 || a >= _nodePositions.Length || b < 0 || b >= _nodePositions.Length)
            return;

        Vector3 worldA = transform.TransformPoint(_nodePositions[a]);
        Vector3 worldB = transform.TransformPoint(_nodePositions[b]);

        Debug.DrawLine(worldA, worldB, Color.green);
    }

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    //void Start()
    //{
    //    Debug.Log($"FEMObject ready: {name}");
    //
    //    if (visualMesh == null)
    //    {
    //        Debug.LogError("VisualMesh reference is missing.");
    //        return;
    //    }
    //
    //    MeshFilter meshFilter = visualMesh.GetComponentInChildren<MeshFilter>();
    //    if (meshFilter == null)
    //    {
    //        Debug.LogError("No MeshFilter found under VisualMesh.");
    //        return;
    //    }
    //
    //    _mesh = meshFilter.mesh;
    //    _originalVertices = _mesh.vertices;
    //    _deformedVertices = new Vector3[_originalVertices.Length];
    //    _vertexBuffer = new float[_originalVertices.Length * 3];
    //
    //    for (int i = 0; i < _originalVertices.Length; ++i)
    //    {
    //        _deformedVertices[i] = _originalVertices[i];
    //        _vertexBuffer[i * 3 + 0] = _originalVertices[i].x;
    //        _vertexBuffer[i * 3 + 1] = _originalVertices[i].y;
    //        _vertexBuffer[i * 3 + 2] = _originalVertices[i].z;
    //    }
    //
    //    FEMPlugin.InitializePlugin();
    //    FEMPlugin.SetVertices(_vertexBuffer, _originalVertices.Length);
    //}

    // Update is called once per frame
    //void Update()
    //{
    //    if (_mesh == null)
    //        return;
    //
    //    FEMPlugin.DeformVertices(Time.deltaTime);
    //    FEMPlugin.GetVertices(_vertexBuffer, _originalVertices.Length);
    //
    //    for (int i = 0; i < _deformedVertices.Length; ++i)
    //    {
    //        _deformedVertices[i].x = _vertexBuffer[i * 3 + 0];
    //        _deformedVertices[i].y = _vertexBuffer[i * 3 + 1];
    //        _deformedVertices[i].z = _vertexBuffer[i * 3 + 2];
    //    }
    //
    //    _mesh.vertices = _deformedVertices;
    //    _mesh.RecalculateNormals();
    //    _mesh.RecalculateBounds();
    //}
}
