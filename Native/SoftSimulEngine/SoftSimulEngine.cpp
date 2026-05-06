#include "pch.h"
#include "SoftSimulEngine.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include "Math/MathTypes.h"
#include "SimulationTypes.h"
#include "Math/CGSolver.h"
#include "ImplicitSystem.h"

static std::vector<Vec3> g_restNodes;
static std::vector<Node> g_nodes;
static std::vector<Tet> g_tets;
static std::vector<Edge> g_edges;

static std::vector<PointConnection> g_pointConnections;
static ImplicitSystem g_implicitSystem;

//static std::vector<float> g_originalVertices;
//static std::vector<float> g_deformedVertices;

static void AddEdge(int i, int j)
{
    Vec3 d = g_restNodes[j] - g_restNodes[i];
    float restLength = Length(d);
    g_edges.push_back({ i, j, restLength });
}

extern "C"
{
    void InitializePlugin()
    {
        g_restNodes.clear();
        g_nodes.clear();
        g_tets.clear();
        g_edges.clear();
    }

    int GetPluginVersion()
    {
        return 1;
    }

    void CreateTestTet()
    {
        g_restNodes.clear();
        g_nodes.clear();
        g_tets.clear();
        g_edges.clear();

        g_restNodes.push_back({ 0.0f, 0.0f, 0.0f });
        g_restNodes.push_back({ 1.0f, 0.0f, 0.0f });
        g_restNodes.push_back({ 0.5f, 0.0f, 1.0f });
        g_restNodes.push_back({ 0.5f, 1.0f, 0.5f });

        Node n0{};
        n0.position = g_restNodes[0];
        n0.velocity = { 0.0f, 0.0f, 0.0f };
        n0.mass = 1.0f;
        n0.invMass = 1.0f;
        n0.isActive = true;

        Node n1{};
        n1.position = g_restNodes[1];
        n1.velocity = { 0.0f, 0.0f, 0.0f };
        n1.mass = 1.0f;
        n1.invMass = 1.0f;
        n1.isActive = true;

        Node n2{};
        n2.position = g_restNodes[2];
        n2.velocity = { 0.0f, 0.0f, 0.0f };
        n2.mass = 1.0f;
        n2.invMass = 1.0f;
        n2.isActive = true;

        Node n3{};
        n3.position = g_restNodes[3];
        n3.velocity = { 0.0f, 0.0f, 0.0f };
        n3.mass = 1.0f;
        n3.invMass = 1.0f;
        n3.isActive = true;

        g_nodes.push_back(n0);
        g_nodes.push_back(n1);
        g_nodes.push_back(n2);
        g_nodes.push_back(n3);

        g_tets.push_back({ 0, 1, 2, 3 });

        // 6 edges of a tetrahedron
        AddEdge(0, 1);
        AddEdge(0, 2);
        AddEdge(0, 3);
        AddEdge(1, 2);
        AddEdge(1, 3);
        AddEdge(2, 3);

        g_implicitSystem.Initialize(&g_nodes);
        g_pointConnections.clear();

        PointConnection c0{};
        c0.nodeIndex = 0;
        c0.targetPosition = g_restNodes[0];
        c0.stiffness = 200.0f;
        c0.damping = 8.0f;
        c0.isActive = true;

        PointConnection c1{};
        c1.nodeIndex = 1;
        c1.targetPosition = g_restNodes[1];
        c1.stiffness = 200.0f;
        c1.damping = 8.0f;
        c1.isActive = true;

        g_pointConnections.push_back(c0);
        g_pointConnections.push_back(c1);
    }

    void StepImplicit(float dt)
    {
        if (dt <= 0.0f)
            return;

        const Vec3 gravity = { 0.0f, -9.81f, 0.0f };

        VectorX b;
        g_implicitSystem.BuildRhs(b, dt, gravity);

        // Add point connection forces into b
        for (const PointConnection& connection : g_pointConnections)
        {
            if (!connection.isActive)
                continue;

            int i = connection.nodeIndex;
            if (i < 0 || i >= static_cast<int>(g_nodes.size()))
                continue;

            Node& node = g_nodes[i];
            if (!node.isActive || node.invMass <= 0.0f)
                continue;

            Vec3 displacement = node.position - connection.targetPosition;
            Vec3 springForce = displacement * (-connection.stiffness);
            Vec3 dampingForce = node.velocity * (-connection.damping);
            Vec3 totalForce = springForce + dampingForce;

            b[3 * i + 0] += dt * totalForce.x;
            b[3 * i + 1] += dt * totalForce.y;
            b[3 * i + 2] += dt * totalForce.z;
        }

        VectorX deltaV(b.size(), 0.0f);

        CGSettings settings;
        settings.maxIterations = 64;
        settings.tolerance = 1e-6f;

        auto applyA = [dt](const VectorX& x, VectorX& y)
            {
                g_implicitSystem.ApplySystemMatrix(x, y, dt);
            };

        CGResult cg = SolveConjugateGradient(applyA, b, deltaV, settings);

        const int nodeCount = static_cast<int>(g_nodes.size());
        for (int i = 0; i < nodeCount; ++i)
        {
            Node& node = g_nodes[i];

            if (!node.isActive || node.invMass <= 0.0f)
                continue;

            node.velocity.x += deltaV[3 * i + 0];
            node.velocity.y += deltaV[3 * i + 1];
            node.velocity.z += deltaV[3 * i + 2];

            node.position = node.position + node.velocity * dt;
        }
    }

    void StepSimulation(float dt)
    {
        if (dt <= 0.0f)
            return;

        StepImplicit(dt);
    }

    void AddImpulseToNode(int nodeIndex, float ix, float iy, float iz)
    {
        if (nodeIndex < 0 || nodeIndex >= static_cast<int>(g_nodes.size()))
            return;

        Node& node = g_nodes[nodeIndex];

        if (!node.isActive || node.invMass <= 0.0f)
            return;

        node.velocity.x += ix;
        node.velocity.y += iy;
        node.velocity.z += iz;
    }

    int GetNodeCount()
    {
        return static_cast<int>(g_nodes.size());
    }

    void GetNodePositions(float* outPositions, int nodeCount)
    {
        if (outPositions == nullptr || nodeCount <= 0)
            return;

        const int count = min(nodeCount, static_cast<int>(g_nodes.size()));

        for (int i = 0; i < count; ++i)
        {
            outPositions[i * 3 + 0] = g_nodes[i].position.x;
            outPositions[i * 3 + 1] = g_nodes[i].position.y;
            outPositions[i * 3 + 2] = g_nodes[i].position.z;
        }
    }

    int GetTetCount()
    {
        return static_cast<int>(g_tets.size());
    }

    void GetTetIndices(int* outIndices, int tetCount)
    {
        if (outIndices == nullptr || tetCount <= 0)
            return;

        const int availableTets = static_cast<int>(g_tets.size());
        const int count = min(tetCount, availableTets);

        for (int i = 0; i < count; ++i)
        {
            outIndices[i * 4 + 0] = g_tets[i].i0;
            outIndices[i * 4 + 1] = g_tets[i].i1;
            outIndices[i * 4 + 2] = g_tets[i].i2;
            outIndices[i * 4 + 3] = g_tets[i].i3;
        }
    }

    void GetFixedNodeFlags(int* outFlags, int nodeCount)
    {
        if (outFlags == nullptr || nodeCount <= 0)
            return;

        const int count = min(nodeCount, static_cast<int>(g_nodes.size()));

        for (int i = 0; i < count; ++i)
        {
            outFlags[i] = g_nodes[i].isActive ? 1 : 0;
        }
    }

    int GetEdgeCount()
    {
        return static_cast<int>(g_edges.size());
    }

    void GetEdges(int* outEdges, int edgeCount)
    {
        if (outEdges == nullptr || edgeCount <= 0)
            return;

        const int count = min(edgeCount, static_cast<int>(g_edges.size()));

        for (int i = 0; i < count; ++i)
        {
            outEdges[i * 2 + 0] = g_edges[i].i;
            outEdges[i * 2 + 1] = g_edges[i].j;
        }
    }

    void SetVertices(const float* vertices, int vertexCount)
    {
        if (vertices == nullptr || vertexCount <= 0)
            return;

        const int floatCount = vertexCount * 3;

        //g_originalVertices.assign(vertices, vertices + floatCount);
        //g_deformedVertices = g_originalVertices;
    }

    void DeformVertices(float dt)
    {
    }

    void GetVertices(float* outVertices, int vertexCount)
    {
    }

    float StepTest(float dt)
    {
        return 0.0f;
    }

}