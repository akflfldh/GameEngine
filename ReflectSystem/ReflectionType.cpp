#include "ReflectSystem/ClassInfoImpl.h"
#include "ReflectSystem/ReflectionClassInfo.h"
Quad::ClassInfo::ClassInfo(const char *className, size_t classSize, void *(*ctor)(void *), void (*dtor)(void *))
{

    mImpl = new ClassInfoImpl(className, classSize, ctor, dtor);
}

Quad::ClassInfo::~ClassInfo()
{

    delete mImpl;
}

void Quad::ClassInfo::AddProperty(Quad::PropertyInfo *property)
{
    mImpl->AddProperty(property);
}

const Quad::PropertyInfo *Quad::ClassInfo::GetPropertyInfo(const char *propertyName) const
{

    return mImpl->GetPropertyInfo(propertyName);
}

void *Quad::ClassInfo::GetPropertyValuePtr(void *instance, const char *propertyName) const
{

    return mImpl->GetPropertyValuePtr(instance, propertyName);
}

const char *Quad::ClassInfo::GetClassName() const
{
    return mImpl->GetClassName();
}

size_t Quad::ClassInfo::GetClassSize() const
{
    return mImpl->GetClassSize();
}

void *Quad::ClassInfo::CreateInstance(void *mem)
{
    return mImpl->CreateInstance(mem);
}

void Quad::ClassInfo::DestoryInstnace(void *instance)
{
    mImpl->DestoryInstnace(instance);
}

void Quad::ClassInfo::SetBaseClassList(const char *classNameList, size_t num)
{

    mImpl->SetBaseClassList(classNameList, num);
}

bool Quad::ClassInfo::IsBaseClass(const char *baseClassName) const
{

    return mImpl->IsBaseClass(baseClassName);
}