using UnityEngine;
using UnityEngine.InputSystem;

public class FEMObject : MonoBehaviour
{
    [SerializeField] private Transform visualMesh;
    [SerializeField] private Transform mechanicalMeshDebug;
    [SerializeField] private Transform constraintHandles;

    private LineRenderer[] _edgeRenderers;
    private Material _edgeMaterial;

    private float[] _nodePositionBuffer;
    private int[] _tetIndexBuffer;
    private int[] _fixedNodeFlags;
    private int[] _edgeIndexBuffer;

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
        int edgeCount = FEMPlugin.GetEdgeCount();

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
        _edgeIndexBuffer = new int[edgeCount * 2];

        FEMPlugin.GetNodePositions(_nodePositionBuffer, nodeCount);
        FEMPlugin.GetTetIndices(_tetIndexBuffer, tetCount);
        FEMPlugin.GetFixedNodeFlags(_fixedNodeFlags, nodeCount);
        FEMPlugin.GetEdges(_edgeIndexBuffer, edgeCount);

        CreateNodeDebugObjects();
        UpdateNodePositionsFromBuffer();
        ApplyNodePositionsToDebugObjects();

        CreateEdgeRenderers(edgeCount);
        UpdateEdgeRenderers();

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
        UpdateEdgeRenderers();
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

    private void CreateEdgeRenderers(int edgeCount)
    {
        if (mechanicalMeshDebug == null)
        {
            Debug.LogError("FEMObject: MechanicalMesh_Debug reference is missing.");
            return;
        }

        _edgeRenderers = new LineRenderer[edgeCount];

        if (_edgeMaterial == null)
        {
            Shader shader = Shader.Find("Sprites/Default");
            _edgeMaterial = new Material(shader);
            _edgeMaterial.color = Color.green;
        }

        for (int i = 0; i < edgeCount; ++i)
        {
            GameObject edgeObject = new GameObject($"Edge_{i}");
            edgeObject.transform.SetParent(mechanicalMeshDebug, false);

            LineRenderer lr = edgeObject.AddComponent<LineRenderer>();
            lr.positionCount = 2;
            lr.useWorldSpace = false;
            lr.startWidth = 0.03f;
            lr.endWidth = 0.03f;
            lr.material = _edgeMaterial;
            lr.startColor = Color.green;
            lr.endColor = Color.green;

            _edgeRenderers[i] = lr;
        }
    }

    private void UpdateEdgeRenderers()
    {
        if (_edgeRenderers == null || _edgeIndexBuffer == null)
            return;

        int edgeCount = _edgeIndexBuffer.Length / 2;

        for (int e = 0; e < edgeCount; ++e)
        {
            int a = _edgeIndexBuffer[e * 2 + 0];
            int b = _edgeIndexBuffer[e * 2 + 1];

            if (a < 0 || a >= _nodePositions.Length || b < 0 || b >= _nodePositions.Length)
                continue;

            LineRenderer lr = _edgeRenderers[e];
            if (lr == null)
                continue;

            lr.SetPosition(0, _nodePositions[a]);
            lr.SetPosition(1, _nodePositions[b]);
        }
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
