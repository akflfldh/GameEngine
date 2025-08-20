#include "ReflectSystem/ReflectionSystem.h"
#include "ReflectSystem/ReflectionSystemImpl.h"
// #include"ReflectionClassInfo.h"

Quad::ReflectionSystemImpl *Quad::ReflectionSystem::mImpl = nullptr;

Quad::ReflectionSystem *Quad::ReflectionSystem::GetInstance()
{
    static ReflectionSystem instance;

    return &instance;
}
Quad::ClassInfo *Quad::ReflectionSystem::CreateClassInfo(const char *ClassName, size_t classSize, void *(*ctor)(void *),
                                                         void (*dtor)(void *))
{
    return mImpl->CreateClassInfo(ClassName, classSize, ctor, dtor);
}

void Quad::ReflectionSystem::AddProperty(Quad::ClassInfo *classInfo, const Quad::PropertyInfo &propertyInfo)
{

    mImpl->AddProperty(classInfo, propertyInfo);
}

void *Quad::ReflectionSystem::CreateClassInstance(const char *className, void *mem)
{

    return mImpl->CreateClassInstance(className, mem);
}

void Quad::ReflectionSystem::DestoryClassInstance(const char *className, void *instance)
{

    mImpl->DestoryClassInstance(className, instance);
}

size_t Quad::ReflectionSystem::GetClassSize(const char *className) const
{

    return mImpl->GetClassSize(className);
}

Quad::ClassInfo *Quad::ReflectionSystem::GetClassInfo(const char *className) const
{

    return mImpl->GetClassInfo(className);
}

Quad::ReflectionSystem::ReflectionSystem()
{

    static ReflectionSystemImpl impl;

    mImpl = &impl;
}
