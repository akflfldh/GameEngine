#pragma once

#include <Utility/UniqueID.h>
#include <string>
#include <unordered_map>
#include <vector>
namespace Core
{
class Component;
class Object;

class ObjectImpl
{
  public:
    ObjectImpl(Object *interfaceObject);
    ~ObjectImpl();

    void Start();

    void SetUniqueID(const CoreUtility::UniqueID &id);
    CoreUtility::UniqueID GetUniqueID() const;

    void SetObjectName(const char *objectName);
    const char *GetObjectName() const;

    Component *CreateComponent(const char *comClassName, const char *comInstanceName);
    Component *GetComponent(const char *comInstanceName) const;

  private:
    void *GetMemory(size_t size);
    void ReleaseMemory(void *);

    void ReleaseComponent(Component *com);

  private:
    std::vector<Component *> mComList;
    std::unordered_map<std::string, Component *> mComNameTable;
    CoreUtility::UniqueID mObjectUniqueID;

    std::string mObjectName;
    Object *mInterfaceObject;
};
} // namespace Core
