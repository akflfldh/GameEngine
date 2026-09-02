#pragma once

#include <CoreMath/CoreMath.h>

namespace CoreMath
{

struct AABB
{
    CoreMath::Vector3 mMin;
    CoreMath::Vector3 mMax;

    void Clear()
    {

        mMin = {FLT_MAX, FLT_MAX, FLT_MAX};
        mMax = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
    }

    // 다음 pos를 포함하도록 AABB를 늘린다.
    void Expand(const CoreMath::Vector3 &pos)
    {

        if (mMin.X > pos.X)
            mMin.X = pos.X;

        if (mMin.Y > pos.Y)
            mMin.Y = pos.Y;

        if (mMin.Z > pos.Z)
            mMin.Z = pos.Z;

        if (mMax.X < pos.X)
            mMax.X = pos.X;

        if (mMax.Y < pos.Y)
            mMax.Y = pos.Y;

        if (mMax.Z < pos.Z)
            mMax.Z = pos.Z;
    }
};

// Ray는 반직선
struct Ray
{
    CoreMath::Vector3 mOrigin;
    CoreMath::Vector3 mDirection;
};

// oMinT : 충돌시 가장 가까운지점의 매개변수 T
inline bool IntersectRayAABB(const Ray &ray, const AABB &aabb, float &oMinT)
{
    float tStart = 0.0f;
    float tEnd = FLT_MAX;

    if (ray.mDirection.X == 0.0f)
    {
        if ((aabb.mMin.X > ray.mOrigin.X) || (aabb.mMax.X < ray.mOrigin.X))
            return false;
    }
    else
    {
        float tStartX = (aabb.mMin.X - ray.mOrigin.X) / ray.mDirection.X;
        float tEndX = (aabb.mMax.X - ray.mOrigin.X) / ray.mDirection.X;

        if (tStartX > tEndX)
            std::swap(tStartX, tEndX);

        if (tStart < tStartX)
            tStart = tStartX;
        if (tEnd > tEndX)
            tEnd = tEndX;

        if (tStart > tEnd)
            return false;
    }

    if (ray.mDirection.Y == 0.0f)
    {
        if ((aabb.mMin.Y > ray.mOrigin.Y) || (aabb.mMax.Y < ray.mOrigin.Y))
            return false;
    }
    else
    {
        float tStartY = (aabb.mMin.Y - ray.mOrigin.Y) / ray.mDirection.Y;
        float tEndY = (aabb.mMax.Y - ray.mOrigin.Y) / ray.mDirection.Y;

        if (tStartY > tEndY)
            std::swap(tStartY, tEndY);

        if (tStart < tStartY)
            tStart = tStartY;
        if (tEnd > tEndY)
            tEnd = tEndY;

        if (tStart > tEnd)
            return false;
    }

    if (ray.mDirection.Z == 0.0f)
    {
        if ((aabb.mMin.Z > ray.mOrigin.Z) || (aabb.mMax.Z < ray.mOrigin.Z))
            return false;
    }
    else
    {
        float tStartZ = (aabb.mMin.Z - ray.mOrigin.Z) / ray.mDirection.Z;
        float tEndZ = (aabb.mMax.Z - ray.mOrigin.Z) / ray.mDirection.Z;

        if (tStartZ > tEndZ)
            std::swap(tStartZ, tEndZ);

        if (tStart < tStartZ)
            tStart = tStartZ;
        if (tEnd > tEndZ)
            tEnd = tEndZ;

        if (tStart > tEnd)
            return false;
    }

    oMinT = tStart;
    return true;
}

inline bool IntersectRayPlane(const Ray &ray, const CoreMath::Vector3 &planeNormal, const CoreMath::Vector3 &planePoint,
                              float &oT)
{
    float denominator = planeNormal.Dot(ray.mDirection);

    // ray 방향과 평면이 거의평행하다면 교차 X
    if (std::abs(denominator) < 0.000001f)
    {
        return false;
    }

    CoreMath::Vector3 planeToOrigin = planePoint - ray.mOrigin;
    float numerator = CoreMath::Dot(planeNormal, planeToOrigin);

    float t = numerator / denominator;

    if (t < 0.0f)
    {
        return false;
    }

    oT = t;
    return true;
}

} // namespace CoreMath