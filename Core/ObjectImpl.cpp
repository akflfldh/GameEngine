#include "ObjectImpl.h"
#include <Core/Component.h>
#include <ReflectSystem/ReflectionSystem.h>
Core::ObjectImpl::ObjectImpl(Object *interfaceObject) : mInterfaceObject(interfaceObject) {}

Core::ObjectImpl::~ObjectImpl()
{

    for (auto it = mComList.rbegin(); it != mComList.rend(); ++it)
    {
        ReleaseComponent(*it);
    }
}

void Core::ObjectImpl::Start()
{

    for (auto com : mComList)
    {
        com->Start();
    }
}

void Core::ObjectImpl::SetUniqueID(const CoreUtility::UniqueID &id)
{
    mObjectUniqueID = id;
}

CoreUtility::UniqueID Core::ObjectImpl::GetUniqueID() const
{
    return mObjectUniqueID;
}

void Core::ObjectImpl::SetObjectName(const char *objectName)
{

    mObjectName = objectName;
}
const char *Core::ObjectImpl::GetObjectName() const
{

    return mObjectName.c_str();
}

Core::Component *Core::ObjectImpl::CreateComponent(const char *comClassName, const char *comInstanceName)
{

    std::unordered_map<std::string, Component *>::iterator it = mComNameTable.find(comInstanceName);
    if (it != mComNameTable.end())
    {

        // log
        return nullptr;
    }

    // reflect system 을 통해서 com 생성

    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();

    size_t comSize = reflectionSystem->GetClassSize(comClassName);
    if (comSize == 0)
    {
        return nullptr;
    }

    void *mem = GetMemory(comSize);
    Component *com = (Component *)(reflectionSystem->CreateClassInstance(comClassName, mem));
    if (com == nullptr)
    {
        ReleaseMemory(mem);
        return nullptr;
    }
    // 이름부여
    com->SetInstanceName(comInstanceName);
    com->SetOwnerObject(mInterfaceObject);

    // 테이블 등록
    mComNameTable[comInstanceName] = com;
    mComList.push_back(com);

    return com;
}

Core::Component *Core::ObjectImpl::GetComponent(const char *comInstanceName) const
{
    std::unordered_map<std::string, Component *>::const_iterator it = mComNameTable.find(comInstanceName);
    if (it == mComNameTable.cend())
    {

        // log
        return nullptr;
    }

    return it->second;
}

void Core::ObjectImpl::ReleaseComponent(Component *com)
{

    if (com == nullptr)
        return;

    mComNameTable.erase(com->GetInstanceName());
    mComList.erase(std::find(mComList.begin(), mComList.end(), com));

    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();

    reflectionSystem->DestoryClassInstance(com->GetClassName(), com);

    ReleaseMemory(com);
}

void *Core::ObjectImpl::GetMemory(size_t size)
{
    // 후에 커스텀할당자코드 삽입
    return new char[size];
}

void Core::ObjectImpl::ReleaseMemory(void *pMem)
{

    delete[] pMem;
}
