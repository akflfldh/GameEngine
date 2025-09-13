#pragma once

#include <Core/CoreDllExport.h>
#include <ReflectSystem/ReflectionMacro.h>
#include <Utility/UniqueID.h>
namespace Core
{
class Entity;
class CORE_API_LIB Map
{
  public:
    Map();
    virtual ~Map() = 0;

    virtual void Start() = 0;
    virtual void Update() = 0;

    template <typename T> T *CreateEntity(const char *entityInstanceName = "");

    virtual void DestoryEntity(Entity *entity) = 0;

    virtual Entity *GetEntity(CoreUtility::UniqueID id) const = 0;

    virtual Entity *GetEntity(const char *entityInstanceName) const = 0;

  protected:
    virtual Entity *CreateEntity(const char *entityClassName, const char *entityInstanceName) = 0;

  private:
};
template <typename T> inline T *Map::CreateEntity(const char *entityInstanceName)
{
    return static_cast<T *>(CreateEntity(T::GetStaticClassName(), entityInstanceName));
}
} // namespace Core
