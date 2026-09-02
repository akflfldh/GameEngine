#include "EditorHitProxyComponent.h"
#include <CoreMath/Geometry.h>

EditorHitProxyComponent::EditorHitProxyComponent() {}

EditorHitProxyComponent ::~EditorHitProxyComponent() {}

bool EditorHitProxyComponent::Raycast(const CoreMath::Ray &ray, Core::HitResult &oHitResult)
{

    CoreMath::AABB aabb = BuildWorldAABB();

    float minT = 0.0F;
    bool bHit = CoreMath::IntersectRayAABB(ray, aabb, minT);

    if (bHit == false)
        return false;

    oHitResult.bHit = bHit;
    oHitResult.mHitComponent = this;
    oHitResult.mDistance = minT;

    return true;
}

void EditorHitProxyComponent::DrawDebugCollider() {}

CoreMath::AABB EditorHitProxyComponent::BuildWorldAABB() const
{

    CoreMath::Vector3 posWorld = GetPositionWorld();
    CoreMath::AABB aabb({posWorld - mHalfExtent}, {posWorld + mHalfExtent});

    return aabb;
}

void EditorHitProxyComponent::SetHalfExtent(const CoreMath::Vector3 &halfExtent)
{

    mHalfExtent = halfExtent;
}
