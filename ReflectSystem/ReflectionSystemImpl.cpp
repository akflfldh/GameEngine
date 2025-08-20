#include "ReflectSystem/ReflectionSystemImpl.h"
#include "ReflectSystem/ClassInfoImpl.h"
#include "ReflectSystem/ReflectionPropertyInfo.h"

Quad::ReflectionSystemImpl::ReflectionSystemImpl() {}

Quad::ReflectionSystemImpl::~ReflectionSystemImpl() {}

void Quad::ReflectionSystemImpl::AddProperty(ClassInfo *classInfo, const PropertyInfo &propertyInfo)
{

    PropertyInfo *pPropertyInfo = new PropertyInfo(propertyInfo);

    classInfo->AddProperty(pPropertyInfo);
}

Quad::ClassInfo *Quad::ReflectionSystemImpl::CreateClassInfo(const char *className, size_t classSize,
                                                             void *(*ctor)(void *), void (*dtor)(void *))
{

    ClassInfo *classInfo = new ClassInfo(className, classSize, ctor, dtor);

    mClassInfoVector.push_back(classInfo);
    mClassInfoTable[className] = classInfo;

    return classInfo;
}

void *Quad::ReflectionSystemImpl::CreateClassInstance(const char *className, void *mem)
{

    std::unordered_map<std::string, ClassInfo *>::iterator it = mClassInfoTable.find(className);

    if (it == mClassInfoTable.end())
        return nullptr;

    ClassInfo *classInfo = it->second;

    return classInfo->CreateInstance(mem);
}

void Quad::ReflectionSystemImpl::DestoryClassInstance(const char *className, void *instance)
{

    std::unordered_map<std::string, ClassInfo *>::iterator it = mClassInfoTable.find(className);

    if (it == mClassInfoTable.end())
        return;

    ClassInfo *classInfo = it->second;

    classInfo->DestoryInstnace(instance);
}

size_t Quad::ReflectionSystemImpl::GetClassSize(const char *className) const
{

    std::unordered_map<std::string, ClassInfo *>::const_iterator it = mClassInfoTable.find(className);

    if (it == mClassInfoTable.end())
        return 0;

    ClassInfo *classInfo = it->second;

    return classInfo->GetClassSize();
}

Quad::ClassInfo *Quad::ReflectionSystemImpl::GetClassInfo(const char *className) const
{

    std::unordered_map<std::string, ClassInfo *>::const_iterator it = mClassInfoTable.find(className);

    if (it == mClassInfoTable.end())
        return nullptr;

    ClassInfo *classInfo = it->second;

    return classInfo;
}