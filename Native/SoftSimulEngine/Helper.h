#pragma once

inline int DofX(int nodeIndex) { return 3 * nodeIndex + 0; }
inline int DofY(int nodeIndex) { return 3 * nodeIndex + 1; }
inline int DofZ(int nodeIndex) { return 3 * nodeIndex + 2; }

inline int TotalDofCount(int nodeCount)
{
    return 3 * nodeCount;
}