#pragma once

#include "ReflectSystemDllMacro.h"
#include "ReflectionPropertyInfo.h"
#include <string>
#include <vector>
namespace Quad
{
struct PropertyInfo;

class REFLECT_SYSTEM_API ClassInfoImpl
{

  public:
    ClassInfoImpl(const char *ClassName, size_t classSize, void *(*ctor)(void *), void (*dtor)(void *));

    ~ClassInfoImpl();

    void AddProperty(PropertyInfo *property);

    const PropertyInfo *GetPropertyInfo(const char *propertyName) const;

    void *GetPropertyValuePtr(void *instance, const char *propertyName) const;

    const char *GetTypeName() const;

    size_t GetClassSize() const;

    void *CreateInstance(void *mem);

    void DestoryInstnace(void *instance);

    void SetBaseClassList(const char *classNameList, size_t num);
    bool IsBaseClass(const char *baseClassName) const;



  private:
    const char *mClassName;
    size_t mClassSize;
    std::vector<PropertyInfo *> mPropertyVector;
    std::vector<std::string> mBaseClassList;

    void *(*mCtor)(void *mem);
    void (*mDtor)(void *);
};
} // namespace Quad
