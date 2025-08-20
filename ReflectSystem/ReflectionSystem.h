#pragma once

#include "ReflectSystem/ReflectSystemDllMacro.h"
#include "ReflectSystem/ReflectionPropertyInfo.h"
#include <cstddef>

namespace Quad
{
class ClassInfo;
class ReflectionSystemImpl;

class REFLECT_SYSTEM_API ReflectionSystem
{

  public:
    static ReflectionSystem *GetInstance();

    ClassInfo *CreateClassInfo(const char *ClassName, size_t classSize, void *(*ctor)(void *), void (*dtor)(void *));

    void AddProperty(ClassInfo *classInfo, const PropertyInfo &propertyInfo);

    void *CreateClassInstance(const char *className, void *mem);
    void DestoryClassInstance(const char *className, void *instance);

    size_t GetClassSize(const char *className) const;

    ClassInfo *GetClassInfo(const char *className) const;

  protected:
    ReflectionSystem();

  private:
    static ReflectionSystemImpl *mImpl;
};

} // namespace Quad
