#pragma once
#include <Core/ObjectTypeDef.h>
#include <queue>
#include <vector>

class Object;

namespace Core
{

struct ObjectSlot
{
    uint64_t mGeneration = 0;
    Object *mObject = nullptr;
};

class ObjectManager
{
  public:
    static ObjectManager *GetInstance();
    ~ObjectManager() = default;

    ObjectHandle Register(Object *object);

    // 등록되지않은 index라면 false 리턴
    bool UnRegister(uint64_t index);

    Object *Get(const ObjectHandle &handle) const;

  private:
    ObjectManager();

    std::vector<ObjectSlot> mObjectSlotList;

    std::queue<uint64_t> mObjectFreeIndexPool;
    uint64_t mNextPoolIndex;
};

} // namespace Core