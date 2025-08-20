#pragma once

#include "ReflectSystem/ReflectSystemDllMacro.h"
#include "ReflectSystem/ReflectionClassInfo.h"
#include "ReflectSystem/ReflectionPropertyInfo.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace Quad
{
struct PropertyInfo;

class REFLECT_SYSTEM_API ReflectionSystemImpl
{
  public:
    ReflectionSystemImpl();
    ~ReflectionSystemImpl();

    void AddProperty(ClassInfo *classInfo, const PropertyInfo &propertyInfo);

    ClassInfo *CreateClassInfo(const char *ClassName, size_t classSize, void *(*ctor)(void *), void (*dtor)(void *));

    void *CreateClassInstance(const char *className, void *mem);
    void DestoryClassInstance(const char *className, void *instance);

    size_t GetClassSize(const char *className) const;
    ClassInfo *GetClassInfo(const char *className) const;

  private:
    std::vector<ClassInfo *> mClassInfoVector;
    std::unordered_map<std::string, ClassInfo *> mClassInfoTable;
};

} // namespace Quad