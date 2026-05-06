#pragma once
#pragma once

#include <vector>
#include <cmath>
#include <cassert>

struct Vec3
{
    float x, y, z;
};

inline Vec3 operator+(const Vec3& a, const Vec3& b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
inline Vec3 operator-(const Vec3& a, const Vec3& b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
inline Vec3 operator*(const Vec3& v, float s) { return { v.x * s, v.y * s, v.z * s }; }
inline Vec3 operator/(const Vec3& v, float s) { return { v.x / s, v.y / s, v.z / s }; }

inline Vec3& operator+=(Vec3& a, const Vec3& b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

inline float Dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float Length(const Vec3& v)
{
    return std::sqrt(Dot(v, v));
}

inline Vec3 Normalize(const Vec3& v)
{
    float len = Length(v);
    if (len < 1e-8f)
        return { 0.0f, 0.0f, 0.0f };
    return v / len;
}

using VectorX = std::vector<float>;

inline void ResizeAndZero(VectorX& v, int n)
{
    v.assign(n, 0.0f);
}

inline float Dot(const VectorX& a, const VectorX& b)
{
    assert(a.size() == b.size());
    float s = 0.0f;
    for (size_t i = 0; i < a.size(); ++i)
        s += a[i] * b[i];
    return s;
}

inline float Norm(const VectorX& v)
{
    return std::sqrt(Dot(v, v));
}



inline void AddScaled(VectorX& y, const VectorX& x, float alpha)
{
    assert(y.size() == x.size());
    for (size_t i = 0; i < y.size(); ++i)
        y[i] += alpha * x[i];
}

inline void ScaleAndAdd(VectorX& out, const VectorX& a, float alpha, const VectorX& b)
{
    assert(a.size() == b.size());
    out.resize(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        out[i] = a[i] + alpha * b[i];
}