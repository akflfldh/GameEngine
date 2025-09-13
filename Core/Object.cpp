#include "Object.h"
#include <Core/ObjectImpl.h>
Core::Object::Object()
{
    // 향후 수정
    mImpl = std::make_unique<ObjectImpl>(this);
}

Core::Object::~Object() {}

const char *Core::Object::GetObjectName() const
{

    return mImpl->GetObjectName();
}

CoreUtility::UniqueID Core::Object::GetUniqueID() const
{

    return mImpl->GetUniqueID();
}

Core::Component *Core::Object::CreateComponent(const char *comClassName, const char *comInstanceName)
{
    return mImpl->CreateComponent(comClassName, comInstanceName);
}

Core::Component *Core::Object::GetComponent(const char *comInstanceName)
{
    return mImpl->GetComponent(comInstanceName);
}

void Core::Object::SetObjectName(const char *objectName)
{

    mImpl->SetObjectName(objectName);
}

void Core::Object::SetObjectUniqueID(const CoreUtility::UniqueID &id)
{

    mImpl->SetUniqueID(id);
}
