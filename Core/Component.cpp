#include "Component.h"

Core::Component::Component() {}

Core::Component::~Component() {}

Core::Object *Core::Component::GetOwnerObject() const
{
    return mOwnerObject;
}

void Core::Component::SetOwnerObject(Object *ownerObject)
{
    mOwnerObject = ownerObject;
}

const char *Core::Component::GetInstanceName() const
{
    return mInstanceName;
}

void Core::Component::SetInstanceName(const char *name)
{

    mInstanceName = name;
}
