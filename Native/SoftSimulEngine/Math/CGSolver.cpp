#include "pch.h"
#include "CGSolver.h"

CGResult SolveConjugateGradient(
    const ApplyOperatorFn& applyA,
    const VectorX& b,
    VectorX& x,
    const CGSettings& settings)
{
    CGResult result{};

    const int n = static_cast<int>(b.size());
    if (n == 0)
    {
        result.converged = true;
        return result;
    }

    if (static_cast<int>(x.size()) != n)
        x.assign(n, 0.0f);

    VectorX Ax(n, 0.0f);
    VectorX r(n, 0.0f);
    VectorX p(n, 0.0f);
    VectorX Ap(n, 0.0f);

    applyA(x, Ax);

    for (int i = 0; i < n; ++i)
        r[i] = b[i] - Ax[i];

    p = r;

    float rsOld = Dot(r, r);
    result.residualNorm = std::sqrt(rsOld);

    if (result.residualNorm < settings.tolerance)
    {
        result.converged = true;
        return result;
    }

    for (int iter = 0; iter < settings.maxIterations; ++iter)
    {
        applyA(p, Ap);

        const float denom = Dot(p, Ap);
        if (std::fabs(denom) < 1e-12f)
            break;

        const float alpha = rsOld / denom;

        for (int i = 0; i < n; ++i)
            x[i] += alpha * p[i];

        for (int i = 0; i < n; ++i)
            r[i] -= alpha * Ap[i];

        const float rsNew = Dot(r, r);
        result.residualNorm = std::sqrt(rsNew);
        result.iterations = iter + 1;

        if (result.residualNorm < settings.tolerance)
        {
            result.converged = true;
            return result;
        }

        const float beta = rsNew / rsOld;

        for (int i = 0; i < n; ++i)
            p[i] = r[i] + beta * p[i];

        rsOld = rsNew;
    }

    return result;
}