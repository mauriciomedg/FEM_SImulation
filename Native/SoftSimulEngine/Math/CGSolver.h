#pragma once
#pragma once

#include "MathTypes.h"
#include <functional>

struct CGSettings
{
    int maxIterations = 128;
    float tolerance = 1e-5f;
};

struct CGResult
{
    bool converged = false;
    int iterations = 0;
    float residualNorm = 0.0f;
};

using ApplyOperatorFn = std::function<void(const VectorX& x, VectorX& y)>;

CGResult SolveConjugateGradient(
    const ApplyOperatorFn& applyA,
    const VectorX& b,
    VectorX& x,
    const CGSettings& settings);