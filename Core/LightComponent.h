#pragma once

#include <Core/CoreType.h>
#include <Core/IRenderProxyManager.h>
#include <Core/SceneComponent.h>
#include <CoreMath/CoreMath.h>

#include "LightComponent.generated.h"

class CORE_API_LIB REFLECT_CLASS(EngineClass) LightComponent : public SceneComponent
{
    GENERATED_BODY(LightComponent)
  public:
    LightComponent();
    virtual ~LightComponent();

    void SetLightType(Core::ELightType type);

    Core::ELightType GetLightType() const;

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

    virtual void OnActiveStateChanged(bool state) override;

    virtual void FlushPropertyDirty() override;

    virtual void Serialize(Arch &arch) override;

  protected:
    virtual void EndTick(float deltaTime) override;
    void UpdateProxy();

  private:
    Core::ELightType mLightType;

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
