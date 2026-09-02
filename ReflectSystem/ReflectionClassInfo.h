#pragma once

#include "ReflectSystem/ReflectSystemDllMacro.h"
#include <ReflectSystem/ReflectionPropertyInfo.h>
#include <string>
#include <vector>
namespace Quad
{
class PropertyInfo;

class REFLECT_SYSTEM_API ClassInfo
{
  public:
    ClassInfo(const char *ClassName, size_t classSize, void *(*ctor)(void *), void (*dtor)(void *));

    ~ClassInfo();

    void AddProperty(PropertyInfo *property);

    PropertyInfo *GetDeclaredPropertyInfo(const char *propertyName) const;

    // 전체 계층 탐색
    PropertyInfo *GetPropertyInfo(const char *propertyName) const;

    void *GetPropertyValuePtr(void *instance, const char *propertyName) const;

    const char *GetTypeName() const;

    size_t GetClassSize() const;

    void *CreateInstance(void *mem);

    void DestoryInstnace(void *instance);

    void SetBaseClassList(const char *classNameList, size_t num);
    bool IsBaseClass(const char *baseClassName) const;

    const std::vector<PropertyInfo *> &GetDeclaredPropertyVector() const;

    void SetParentClassList(const char *parentClassNameList, size_t num);
    const std::vector<std::string> &GetParentClassList() const;

    void SetAncestorClassList(const char *ancestorList, size_t num);
    bool IsAncestorClass(const char *ancestorClassName) const;

    void SetAlignment(size_t alignment);
    size_t GetAlignment() const;

    void SetParentClassInfo(ClassInfo *info);
    ClassInfo *GetParentClassInfo() const;

    void SetEngineClass(bool flag);
    bool IsEngineClass() const;

  private:
    const char *mClassName;
    size_t mClassSize;
    size_t mAlignment;
    std::vector<PropertyInfo *> mPropertyVector;
    std::vector<std::string> mBaseClassList;
    std::vector<std::string> mParentClassList;
    std::vector<std::string> mAncestorClassList;

    void *(*mCtor)(void *mem);
    void (*mDtor)(void *);

    ClassInfo *mParentClassInfo;

    bool mIsEngineClass;
};

} // namespace Quad
