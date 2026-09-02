#pragma once
#include <Core/CoreDllExport.h>
#include <Core/CoreType.h>
#include <Core/ObjectPtr.h>
#include <InputSystem/IInputHandler.h>

#include "ObjectController.generated.h"

class ControllableEntity;

class CORE_API_LIB REFLECT_CLASS(EngineClass) ObjectController : public Object, public Quad::IInputHandler
{
    GENERATED_BODY(ObjectController)

  public:
    ObjectController();
    virtual ~ObjectController() = 0;

    virtual void Tick(float deltaTime);

    virtual void CheckToggleInput(const Core::InputData &inputData) = 0;
    bool GetActiveState() const;

    void Possess(ControllableEntity *object);
    void UnPossess();

    Object *GetPossessObject() const;

    virtual bool HandleInput(const Quad::RawInputData &inputData) override;
    virtual bool HandleInput(const Core::InputData &inputData) = 0;

  protected:
    virtual void OnPossess(ControllableEntity *object);
    virtual void OnUnPossess();

    bool mActiveState;

  private:
    Core::ObjectPtr<ControllableEntity> mObject;
};
