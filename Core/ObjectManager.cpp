#include "Core/ObjectManager.h"

Core::ObjectManager *Core::ObjectManager::GetInstance()
{

    static ObjectManager instance;

    return &instance;
}

Core::ObjectManager::ObjectManager() : mNextPoolIndex(0) {}

Core::ObjectHandle Core::ObjectManager::Register(Object *object)
{
    uint64_t objectPoolIndex = mNextPoolIndex;
    if (mObjectFreeIndexPool.empty())
    {
        mObjectSlotList.push_back({});
        mNextPoolIndex++;
    }
    else
    {
        objectPoolIndex = mObjectFreeIndexPool.front();
        mObjectFreeIndexPool.pop();
    }
    mObjectSlotList[objectPoolIndex].mGeneration++;
    mObjectSlotList[objectPoolIndex].mObject = object;
    ObjectHandle handle = {objectPoolIndex, mObjectSlotList[objectPoolIndex].mGeneration};

    return handle;
}

bool Core::ObjectManager::UnRegister(uint64_t index)
{
    if (mObjectSlotList[index].mObject == nullptr)
        return false;

    mObjectFreeIndexPool.push(index);
    mObjectSlotList[index].mObject = nullptr;

    return true;
}

Object *Core::ObjectManager::Get(const ObjectHandle &handle) const
{

    Object *object = mObjectSlotList[handle.mObjectPoolIndex].mObject;
    if (object == nullptr)
        return nullptr;

    return mObjectSlotList[handle.mObjectPoolIndex].mGeneration == handle.mGeneration ? object : nullptr;
}
