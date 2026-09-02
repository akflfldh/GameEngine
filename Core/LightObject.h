#pragma once

#include <Core/CoreType.h>
#include <Core/Entity.h>

#include "LightObject.generated.h"

class LightComponent;
namespace Core
{

class CORE_API_LIB REFLECT_CLASS(EngineClass) LightObject : public Entity

{
    GENERATED_BODY(LightObject)

  public:
    LightObject();
    virtual ~LightObject();

    virtual void OnBegin() override;
    virtual void Update(float DeltaTime);

    void SetLightType(ELightType type);

    ELightType GetLightType() const;

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

    virtual void EndTick(float deltaTime) override;

    virtual void Serialize(Arch &arch) override;

    REFLECT_PROPERTY()
    LightComponent *mLightComponent;

  private:
};
} // namespace Core
