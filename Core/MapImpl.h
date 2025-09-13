#pragma once

#include <Core/Map.h>
#include <Utility/UniqueID.h>
#include <string>
#include <unordered_map>
#include <vector>
namespace Quad
{
class ReflectionSystem;
}

namespace Core
{

class Entity;
class MapImpl : public Map
{

  public:
    MapImpl();
    virtual ~MapImpl();

    virtual void Start() override;
    virtual void Update(/*시간*/) override;

    virtual void DestoryEntity(Entity *entity) override;

    virtual Entity *GetEntity(CoreUtility::UniqueID id) const override;
    virtual Entity *GetEntity(const char *entityInstanceName) const override;

  protected:
    virtual Entity *CreateEntity(const char *entityClassName, const char *entityInstanceName) override;

  private:
    void *GetMemory(size_t size);
    void ReleaseMemory(void *mem);

  private:
    std::vector<Entity *> mEntityList;
    std::unordered_map<std::string, Entity *> mEntityNameTable;
    std::unordered_map<CoreUtility::UniqueID, Entity *> mEntityIDTable;
    Quad::ReflectionSystem *mReflectionSystem;
};

} // namespace Core