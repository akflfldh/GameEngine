#pragma once

#include <Core/CoreType.h>
#include <Core/IRenderProxyManager.h>
#include <Core/SceneComponent.h>
#include <CoreMath/CoreMath.h>

#include "LightComponent.generated.h"

/// 조명 속성과 CPU LightProxy를 소유하며, 렌더러에는 프록시를 통해 상태를 전달한다.
/// 개별 조명의 켜짐 상태는 Owner Object의 active와 독립적이며 GPU 리소스는 소유하지 않는다.
class CORE_API_LIB REFLECT_CLASS(EngineClass) LightComponent : public SceneComponent
{
    GENERATED_BODY(LightComponent)
  public:
    LightComponent();
    virtual ~LightComponent();

    void SetLightType(Core::ELightType type);

    Core::ELightType GetLightType() const;

    void SetLightEnabled(bool enabled);
    bool GetLightEnabled() const;

    CoreMath::Vector3 GetStrength() const;
    //  CoreMath::Vector3 GetDirection() const;
    float GetFalloffStart() const;
    float GetFalloffEnd() const;
    float GetSpotPower() const;

    void SetStrength(const CoreMath::Vector3 &strength);
    //  void SetDirection()
    void SetFalloffStart(float value);
    void SetFalloffEnd(float value);
    void SetSpotPower(float value);

    virtual void OnTransformChanged() override;
    virtual void OnOwnerObjectAddedToMap() override;
    virtual void OnOwnerObjectRemovedFromMap() override;

    virtual void OnActiveStateChanged(bool state) override;

    virtual void FlushPropertyDirty() override;

    virtual void Serialize(Arch &arch) override;

  protected:
    virtual void EndTick(float deltaTime) override;
    void UpdateProxy();

  private:
    Core::ELightType mLightType;

    // 런타임 발광 상태다. 기존 저장 형식은 유지하며, Owner 비활성화가 이 값을 변경하지 않는다.
    bool mLightEnabled = true;

    REFLECT_PROPERTY()
    CoreMath::Vector3 mStrength; // 공통
    // CoreMath::Vector3 mDirection; // 평행광, 점적광

    REFLECT_PROPERTY()
    float mFalloffStart = 0.0f; // 점광,점적광

    REFLECT_PROPERTY()
    float mFalloffEnd = 100.0f; // 점광,점점광

    REFLECT_PROPERTY()
    float mSpotPower = 1.0f; // 점적광

    std::unique_ptr<Core::LightProxy> mLightProxy;
};
