#pragma once

#include <Core/CoreDllExport.h>
#include <ReflectSystem/ReflectionMacro.h>
#include <Utility/UniqueID.h>
#include <memory>

#include <Core/Generated/Object.generated.h>
namespace Core
{
class ObjectImpl;
class Component;
class CORE_API_LIB REFLECT_CLASS Object
{
    GENERATED_BODY(Object)
    friend class MapImpl;

  public:
    Object();
    virtual ~Object() = 0;

    virtual void Start() = 0;

    const char *GetObjectName() const;
    CoreUtility::UniqueID GetUniqueID() const;

  protected:
    template <typename T> Component *CreateComponent(const char *comInstanceName);

    Component *GetComponent(const char *comInstanceName);

  private:
    Component *CreateComponent(const char *comClassName, const char *comInstanceName);
    void SetObjectName(const char *objectName);
    void SetObjectUniqueID(const CoreUtility::UniqueID &id);

  private:
    std::unique_ptr<ObjectImpl> mImpl;
};

template <typename T> inline Component *Object::CreateComponent(const char *comInstanceName)
{
    return static_cast<T *>(CreateComponent(T::GetStaticClassName(), comInstanceName));
}

} // namespace Core