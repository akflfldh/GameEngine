#pragma once

#include <Core/IHittable.h>
#include <Core/SceneComponent.h>
#include <CoreMath/Geometry.h>

#include "EditorHitProxyComponent.generated.h"

/*

실체가없는 PlayerStart, Light,Camera 등의 오브젝트들에 대해서  마우스 hit음 대신 담당하는 컴포넌트
이 컴포넌트가 hit되면 해당 비주얼오브젝트들에 바인딩된 source가 hit 되는것으로 처리


*/

class REFLECT_CLASS(EngineClass) EditorHitProxyComponent : public SceneComponent, public Core::IHittable
{
    GENERATED_BODY(EditorHitProxyComponent)
  public:
    EditorHitProxyComponent();
    virtual ~EditorHitProxyComponent();

    virtual bool Raycast(const CoreMath::Ray &ray, Core::HitResult &oHitResult) override;
    virtual void DrawDebugCollider() override;

    void SetHalfExtent(const CoreMath::Vector3 &halfExtent);

  private:
    CoreMath::AABB BuildWorldAABB() const;

  private:
    CoreMath::Vector3 mHalfExtent = {0.5f, 0.5f, 0.5f};
};
