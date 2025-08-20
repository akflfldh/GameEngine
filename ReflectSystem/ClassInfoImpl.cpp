#include "ReflectSystem/ClassInfoImpl.h"
#include "ReflectSystem/ReflectionClassInfo.h"

Quad::ClassInfoImpl::ClassInfoImpl(const char *className, size_t classSize, void *(*ctor)(void *), void (*dtor)(void *))
    : mClassName(className), mClassSize(classSize), mCtor(ctor), mDtor(dtor)
{
}

Quad::ClassInfoImpl::~ClassInfoImpl() {}

void Quad::ClassInfoImpl::AddProperty(Quad::PropertyInfo *property)
{

    mPropertyVector.push_back(property);
}

const Quad::PropertyInfo *Quad::ClassInfoImpl::GetPropertyInfo(const char *propertyName) const
{
    for (PropertyInfo *prop : mPropertyVector)
    {
        if (std::strcmp(prop->mPropertyName, propertyName) == 0)
            return prop;
    }

    return nullptr;
}

void *Quad::ClassInfoImpl::GetPropertyValuePtr(void *instance, const char *propertyName) const
{
    const PropertyInfo *propInfo = GetPropertyInfo(propertyName);
    if (propInfo == nullptr)
        return nullptr;

    return static_cast<char *>(instance) + propInfo->mOffset;
}

const char *Quad::ClassInfoImpl::GetClassName() const
{
    return mClassName;
}
size_t Quad::ClassInfoImpl::GetClassSize() const
{

    return mClassSize;
}

void *Quad::ClassInfoImpl::CreateInstance(void *mem)
{
    return mCtor(mem);
}

void Quad::ClassInfoImpl::DestoryInstnace(void *instance)
{
    mDtor(instance);
}

void Quad::ClassInfoImpl::SetBaseClassList(const char *classNameList, size_t num)
{
    int strIndex = 0;
    for (size_t classIndex = 0; classIndex < num; ++classIndex)
    {
        std::string className;

        while (1)
        {
            if (classNameList[strIndex] == ' ')
            {

                mBaseClassList.push_back(className);
                break;
            }
            else
            {
                className += classNameList[strIndex];
            }

            strIndex++;
        }
    }
}

bool Quad::ClassInfoImpl::IsBaseClass(const char *baseClassName) const
{

    std::vector<std::string>::const_iterator it =
        std::find(mBaseClassList.begin(), mBaseClassList.end(), baseClassName);

    return it == mBaseClassList.cend() ? false : true;
}
