#pragma once

#include <Core/ControllableEntity.h>
#include <Core/CoreType.h>

#include "CameraObject.generated.h"

class CameraComponent;

class CORE_API_LIB REFLECT_CLASS(EngineClass) CameraObject : public ControllableEntity
{
    GENERATED_BODY(CameraObject)
  public:
    CameraObject();
    virtual ~CameraObject();

    virtual void OnBegin() override;
    virtual void Update(float DeltaTime);

    void OnInput(const Core::InputData &inputData);

    CameraComponent *GetCameraComponent() const;

    void SetTestValue(float v);

    virtual void Serialize(Arch &arch) override;

  private:
    REFLECT_PROPERTY()
    CameraComponent *mCameraComponent;
};
