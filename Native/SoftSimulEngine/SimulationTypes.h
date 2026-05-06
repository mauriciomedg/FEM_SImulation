#pragma once

#include "Math/MathTypes.h"

struct Node
{
    Vec3 position;
    Vec3 velocity;
    float mass;
    float invMass;
    bool isActive;
};

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

struct PointConnection
{
    int nodeIndex;
    Vec3 targetPosition;
    float stiffness;
    float damping;
    bool isActive;
};