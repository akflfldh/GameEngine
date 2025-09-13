#include "MapImpl.h"
#include <Core/Entity.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <Utility/Utility.h>
Core::MapImpl::MapImpl()
{

    mReflectionSystem = Quad::ReflectionSystem::GetInstance();
}

Core::MapImpl::~MapImpl() {}

void Core::MapImpl::Start() {}

void Core::MapImpl::Update()
{

    for (auto element : mEntityList)
    {
        element->Update();
    }
}

void Core::MapImpl::DestoryEntity(Entity *entity) {}

Core::Entity *Core::MapImpl::GetEntity(CoreUtility::UniqueID id) const
{
    std::unordered_map<CoreUtility::UniqueID, Entity *>::const_iterator it = mEntityIDTable.find(id);
    if (it == mEntityIDTable.cend())
    {

        return nullptr;
    }

    return it->second;
}

Core::Entity *Core::MapImpl::GetEntity(const char *entityInstanceName) const
{

    std::unordered_map<std::string, Entity *>::const_iterator it = mEntityNameTable.find(entityInstanceName);
    if (it == mEntityNameTable.cend())
    {
        return nullptr;
    }

    return it->second;
}

Core::Entity *Core::MapImpl::CreateEntity(const char *entityClassName, const char *entityInstanceName)
{

    std::string instanceName = entityInstanceName;
    if (instanceName == "")
    {
        // 이름을 entityClassName에기반하여 엔진이 생성해준다.

        std::string tempInstanceName;
        int count = 1;
        do
        {

            tempInstanceName = entityClassName;
            tempInstanceName += std::to_string(count++);
        } while (mEntityNameTable.find(tempInstanceName) != mEntityNameTable.end());

        instanceName = tempInstanceName;
    }
    else
    {
        std::unordered_map<std::string, Entity *>::iterator it = mEntityNameTable.find(instanceName);
        if (it != mEntityNameTable.end())
        {
            // log or 등록해놓은callback호출
            return nullptr;
        }
    }

    size_t classSize = mReflectionSystem->GetClassSize(entityClassName);
    if (classSize == 0)
    {
        // log
        return nullptr;
    }

    void *pMem = GetMemory(classSize);

    Entity *entity = static_cast<Entity *>(mReflectionSystem->CreateClassInstance(entityClassName, pMem));

    if (entity == nullptr)
    {

        ReleaseMemory(pMem);
        return nullptr;
    }

    ((Object *)entity)->SetObjectName(instanceName.c_str());
    ((Object *)entity)->SetObjectUniqueID(CoreUtility::Utility::MakeUniqueID());

    mEntityList.push_back(entity);
    mEntityNameTable[instanceName] = entity;
    mEntityIDTable[entity->GetUniqueID()] = entity;

    return entity;
}

void *Core::MapImpl::GetMemory(size_t size)
{

    void *ptr = new char[size];
    return ptr;
}
void Core::MapImpl::ReleaseMemory(void *mem)
{

    delete[] mem;
    return;
}