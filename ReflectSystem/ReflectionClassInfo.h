#pragma once

#include <vector>

#include "ReflectSystem/ReflectSystemDllMacro.h"

namespace Quad
{
class ClassInfoImpl;
class PropertyInfo;

class REFLECT_SYSTEM_API ClassInfo
{
  public:
    ClassInfo(const char *ClassName, size_t classSize, void *(*ctor)(void *), void (*dtor)(void *));

    ~ClassInfo();

    void AddProperty(PropertyInfo *property);

    const PropertyInfo *GetPropertyInfo(const char *propertyName) const;

    void *GetPropertyValuePtr(void *instance, const char *propertyName) const;

    const char *GetClassName() const;

    size_t GetClassSize() const;

    void *CreateInstance(void *mem);

    void DestoryInstnace(void *instance);

    void SetBaseClassList(const char *classNameList, size_t num);

    bool IsBaseClass(const char *baseClassName) const;

  private:
    ClassInfoImpl *mImpl;
};

} // namespace Quad
