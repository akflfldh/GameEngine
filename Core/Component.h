#pragma once
#include <Core/CoreDllExport.h>
#include <ReflectSystem/ReflectionMacro.h>

#include <Core/Generated/Component.generated.h>

namespace Core
{

class Object;

class CORE_API_LIB REFLECT_CLASS Component
{
    GENERATED_BODY(Component)
    friend class ObjectImpl;

  public:
    Component();
    virtual ~Component() = 0;

    Object *GetOwnerObject() const;

    virtual void Start() = 0;

    const char *GetInstanceName() const;

  private:
    void SetOwnerObject(Object *ownerObject);
    void SetInstanceName(const char *name);

  private:
    Object *mOwnerObject;
    const char *mInstanceName;





};

} // namespace Core