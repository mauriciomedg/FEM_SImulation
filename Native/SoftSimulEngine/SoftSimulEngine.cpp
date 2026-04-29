#include "pch.h"
#include "SoftSimulEngine.h"
#include <vector>
#include <cmath>
#include <algorithm>

struct Vec3
{
    float x, y, z;
};

static Vec3 operator+(const Vec3& a, const Vec3& b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
static Vec3 operator-(const Vec3& a, const Vec3& b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
static Vec3 operator*(const Vec3& v, float s) { return { v.x * s, v.y * s, v.z * s }; }
static Vec3 operator/(const Vec3& v, float s) { return { v.x / s, v.y / s, v.z / s }; }

static Vec3& operator+=(Vec3& a, const Vec3& b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

static float Dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static float Length(const Vec3& v)
{
    return std::sqrt(Dot(v, v));
}

static Vec3 Normalize(const Vec3& v)
{
    float len = Length(v);
    if (len < 1e-8f)
        return { 0.0f, 0.0f, 0.0f };
    return v / len;
}

struct Tet
{
    int i0, i1, i2, i3;
};

struct Edge
{
    int i;
    int j;
    float restLength;
};


struct Node
{
    Vec3 restPosition;
    Vec3 position;
    Vec3 velocity;
    float invMass;
    bool isFixed;
};

static std::vector<Vec3> g_restNodes;
static std::vector<Node> g_nodes;
static std::vector<Tet> g_tets;
static std::vector<Edge> g_edges;


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
        n0.invMass = 0.0f;
        n0.isFixed = true;

        Node n1{};
        n1.position = g_restNodes[1];
        n1.velocity = { 0.0f, 0.0f, 0.0f };
        n1.invMass = 1.0f;
        n1.isFixed = false;

        Node n2{};
        n2.position = g_restNodes[2];
        n2.velocity = { 0.0f, 0.0f, 0.0f };
        n2.invMass = 1.0f;
        n2.isFixed = false;

        Node n3{};
        n3.position = g_restNodes[3];
        n3.velocity = { 0.0f, 0.0f, 0.0f };
        n3.invMass = 1.0f;
        n3.isFixed = false;

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
    }

    void StepSimulation(float dt)
    {
        if (dt <= 0.0f)
            return;

        const Vec3 gravity = { 0.0f, -9.81f, 0.0f };
        const float globalDamping = 0.5f;
        const float edgeStiffness = 80.0f;
        const float edgeDamping = 3.0f;

        const int nodeCount = static_cast<int>(g_nodes.size());
        std::vector<Vec3> forces(nodeCount, { 0.0f, 0.0f, 0.0f });

        for (int i = 0; i < nodeCount; ++i)
        {
            if (!g_nodes[i].isFixed && g_nodes[i].invMass > 0.0f)
            {
                float mass = 1.0f / g_nodes[i].invMass;
                forces[i] += gravity * mass;
                forces[i] += g_nodes[i].velocity * (-globalDamping);
            }
        }

        for (const Edge& edge : g_edges)
        {
            Node& ni = g_nodes[edge.i];
            Node& nj = g_nodes[edge.j];

            Vec3 delta = nj.position - ni.position;
            float currentLength = Length(delta);
            if (currentLength < 1e-8f)
                continue;

            Vec3 dir = delta / currentLength;

            float stretch = currentLength - edge.restLength;

            Vec3 relativeVelocity = nj.velocity - ni.velocity;
            float relVelAlongEdge = Dot(relativeVelocity, dir);

            float springForceMagnitude = edgeStiffness * stretch;
            float dampingForceMagnitude = edgeDamping * relVelAlongEdge;

            Vec3 force = dir * (springForceMagnitude + dampingForceMagnitude);

            forces[edge.i] += force;
            forces[edge.j] += force * (-1.0f);
        }

        for (int i = 0; i < nodeCount; ++i)
        {
            Node& node = g_nodes[i];

            if (node.isFixed || node.invMass <= 0.0f)
            {
                node.position = g_restNodes[i];
                node.velocity = { 0.0f, 0.0f, 0.0f };
                continue;
            }

            Vec3 acceleration = forces[i] * node.invMass;
            node.velocity += acceleration * dt;
            node.position += node.velocity * dt;
        }
    }

    void AddImpulseToNode(int nodeIndex, float ix, float iy, float iz)
    {
        if (nodeIndex < 0 || nodeIndex >= static_cast<int>(g_nodes.size()))
            return;

        Node& node = g_nodes[nodeIndex];

        if (node.isFixed || node.invMass <= 0.0f)
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
        //g_time += dt;
        //
        //if (g_originalVertices.empty())
        //    return;
        //
        //const int vertexCount = static_cast<int>(g_originalVertices.size() / 3);
        //
        //for (int i = 0; i < vertexCount; ++i)
        //{
        //    const int baseIndex = i * 3;
        //
        //    const float x = g_originalVertices[baseIndex + 0];
        //    const float y = g_originalVertices[baseIndex + 1];
        //    const float z = g_originalVertices[baseIndex + 2];
        //
        //    const float offset = 0.1f * std::sinf(g_time * 2.0f + x + z);
        //
        //    g_deformedVertices[baseIndex + 0] = x;
        //    g_deformedVertices[baseIndex + 1] = y + offset;
        //    g_deformedVertices[baseIndex + 2] = z;
        //}
    }

    void GetVertices(float* outVertices, int vertexCount)
    {
        //if (outVertices == nullptr || vertexCount <= 0)
        //    return;
        //
        //const int floatCount = vertexCount * 3;
        //const int availableFloatCount = static_cast<int>(g_deformedVertices.size());
        //
        //const int copyCount = floatCount < availableFloatCount ? floatCount : availableFloatCount; //std::min(floatCount, availableFloatCount);
        //
        //for (int i = 0; i < copyCount; ++i)
        //{
        //    outVertices[i] = g_deformedVertices[i];
        //}
    }

    float StepTest(float dt)
    {
        //g_time += dt;
        //return g_time;
        return 0.0f;
    }

}