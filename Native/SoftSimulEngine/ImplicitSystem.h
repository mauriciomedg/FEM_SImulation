#pragma once
#pragma once

#include "Math/MathTypes.h"
#include <vector>

struct Node;
struct Tet;

class ImplicitSystem
{
public:
    void Initialize(std::vector<Node>* nodes);

    int GetDofCount() const;

    void BuildMassVector(VectorX& massDiag) const;
    void BuildRhs(VectorX& b, float dt, const Vec3& gravity) const;

    void ApplySystemMatrix(const VectorX& x, VectorX& y, float dt) const;
    void ApplyFixedDofMask(VectorX& v) const;

private:
    std::vector<Node>* m_nodes = nullptr;
};