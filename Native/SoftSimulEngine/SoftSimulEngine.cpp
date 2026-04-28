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

struct Tet
{
    int i0, i1, i2, i3;
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

//static std::vector<float> g_originalVertices;
//static std::vector<float> g_deformedVertices;

extern "C"
{
    void InitializePlugin()
    {
        g_restNodes.clear();
        g_nodes.clear();
        g_tets.clear();
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
    }

    void StepSimulation(float dt)
    {
        if (g_nodes.size() != g_restNodes.size())
            return;

        const Vec3 gravity = { 0.0f, -9.81f, 0.0f };
        const float kRest = 25.0f;
        const float damping = 4.0f;

        const int nodeCount = static_cast<int>(g_nodes.size());

        for (int i = 0; i < nodeCount; ++i)
        {
            Node& node = g_nodes[i];

            if (node.isFixed || node.invMass <= 0.0f)
            {
                node.position = g_restNodes[i];
                node.velocity = { 0.0f, 0.0f, 0.0f };
                continue;
            }

            Vec3 displacement = node.position - g_restNodes[i];
            Vec3 restoringForce = displacement * (-kRest);
            Vec3 dampingForce = node.velocity * (-damping);

            Vec3 acceleration = gravity + restoringForce + dampingForce;

            node.velocity = node.velocity + acceleration * dt;
            node.position = node.position + node.velocity * dt;
        }
    }

    int GetNodeCount()
    {
        return static_cast<int>(g_nodes.size());
    }

    void GetNodePositions(float* outPositions, int nodeCount)
    {
        if (outPositions == nullptr || nodeCount <= 0)
            return;

        const int availableNodes = static_cast<int>(g_nodes.size());
        const int count = min(nodeCount, availableNodes);

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