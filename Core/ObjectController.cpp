#include "Core/ObjectController.h"
#include <Core/ControllableEntity.h>
ObjectController::ObjectController() : mActiveState(false) {}

ObjectController::~ObjectController() {}

void ObjectController::Tick(float deltaTime)
{

    Object::Tick(deltaTime);
}

bool ObjectController::GetActiveState() const
{

    return mActiveState;
}

void ObjectController::Possess(ControllableEntity *object)
{
    mObject = object;
    OnPossess(object);
}

void ObjectController::UnPossess()
{
    OnUnPossess();
    mObject = nullptr;
}

Object *ObjectController::GetPossessObject() const
{
    return mObject.Get();
}

void ObjectController::OnPossess(ControllableEntity *ControllableEntity) {}

void ObjectController::OnUnPossess() {}

bool ObjectController::HandleInput(const Quad::RawInputData &inputData)
{
    return false;
}
