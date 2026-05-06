#include "pch.h"
#include "ImplicitSystem.h"
#include "Helper.h"
#include "SimulationTypes.h"


void ImplicitSystem::Initialize(std::vector<Node>* nodes)
{
    m_nodes = nodes;
}

int ImplicitSystem::GetDofCount() const
{
    if (m_nodes == nullptr)
        return 0;
    return static_cast<int>(m_nodes->size()) * 3;
}

void ImplicitSystem::BuildMassVector(VectorX& massDiag) const
{
    const int dofCount = GetDofCount();
    massDiag.assign(dofCount, 0.0f);

    if (m_nodes == nullptr)
        return;

    const int nodeCount = static_cast<int>(m_nodes->size());
    for (int i = 0; i < nodeCount; ++i)
    {
        const Node& node = (*m_nodes)[i];
        if (!node.isActive)
            continue;

        massDiag[DofX(i)] = node.mass;
        massDiag[DofY(i)] = node.mass;
        massDiag[DofZ(i)] = node.mass;
    }
}

void ImplicitSystem::BuildRhs(VectorX& b, float dt, const Vec3& gravity) const
{
    const int dofCount = GetDofCount();
    b.assign(dofCount, 0.0f);

    if (m_nodes == nullptr)
        return;

    const int nodeCount = static_cast<int>(m_nodes->size());
    for (int i = 0; i < nodeCount; ++i)
    {
        const Node& node = (*m_nodes)[i];
        if (!node.isActive)
            continue;

        const Vec3 force = gravity * node.mass;

        b[DofX(i)] = dt * force.x;
        b[DofY(i)] = dt * force.y;
        b[DofZ(i)] = dt * force.z;
    }
}

void ImplicitSystem::ApplySystemMatrix(const VectorX& x, VectorX& y, float dt) const
{
    const int dofCount = GetDofCount();
    y.assign(dofCount, 0.0f);

    if (m_nodes == nullptr)
        return;

    const int nodeCount = static_cast<int>(m_nodes->size());
    for (int i = 0; i < nodeCount; ++i)
    {
        const Node& node = (*m_nodes)[i];

        if (!node.isActive)
            continue;

        const float m = node.mass;

        y[DofX(i)] = m * x[DofX(i)];
        y[DofY(i)] = m * x[DofY(i)];
        y[DofZ(i)] = m * x[DofZ(i)];
    }

    ApplyFixedDofMask(y);
}

void ImplicitSystem::ApplyFixedDofMask(VectorX& v) const
{
    if (m_nodes == nullptr)
        return;

    const int nodeCount = static_cast<int>(m_nodes->size());
    for (int i = 0; i < nodeCount; ++i)
    {
        const Node& node = (*m_nodes)[i];
        if (!node.isActive)
        {
            v[DofX(i)] = 0.0f;
            v[DofY(i)] = 0.0f;
            v[DofZ(i)] = 0.0f;
        }
    }
}