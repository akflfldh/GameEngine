#include "ReflectSystem/ReflectionClassInfo.h"
#include <ReflectSystem/ReflectionPropertyInfo.h>
Quad::ClassInfo::ClassInfo(const char *className, size_t classSize, void *(*ctor)(void *), void (*dtor)(void *))
    : mClassName(className), mClassSize(classSize), mAlignment(0), mCtor(ctor), mDtor(dtor), mParentClassInfo(nullptr),
      mIsEngineClass(false)
{
}

Quad::ClassInfo::~ClassInfo() {}

void Quad::ClassInfo::AddProperty(Quad::PropertyInfo *property)
{

    mPropertyVector.push_back(property);
}

Quad::PropertyInfo *Quad::ClassInfo::GetPropertyInfo(const char *propertyName) const
{
    auto property = GetDeclaredPropertyInfo(propertyName);
    if (property)
        return property;

    if (mParentClassInfo && (mParentClassInfo->IsEngineClass() == false))
    {
        return mParentClassInfo->GetPropertyInfo(propertyName);
    }

    return nullptr;
}

Quad::PropertyInfo *Quad::ClassInfo::GetDeclaredPropertyInfo(const char *propertyName) const
{
    for (PropertyInfo *prop : mPropertyVector)
    {
        if (std::strcmp(prop->mPropertyName, propertyName) == 0)
            return prop;
    }

    return nullptr;
}

void *Quad::ClassInfo::GetPropertyValuePtr(void *instance, const char *propertyName) const
{
    const PropertyInfo *propInfo = GetPropertyInfo(propertyName);
    if (propInfo == nullptr)
        return nullptr;

    return static_cast<char *>(instance) + propInfo->mOffset;
}

const char *Quad::ClassInfo::GetTypeName() const
{
    return mClassName;
}
size_t Quad::ClassInfo::GetClassSize() const
{

    return mClassSize;
}

void *Quad::ClassInfo::CreateInstance(void *mem)
{
    return mCtor(mem);
}

void Quad::ClassInfo::DestoryInstnace(void *instance)
{
    mDtor(instance);
}

void Quad::ClassInfo::SetBaseClassList(const char *classNameList, size_t num)
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
                strIndex++;
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

bool Quad::ClassInfo::IsBaseClass(const char *baseClassName) const
{

    std::vector<std::string>::const_iterator it =
        std::find(mBaseClassList.begin(), mBaseClassList.end(), baseClassName);

    return it == mBaseClassList.cend() ? false : true;
}

const std::vector<Quad::PropertyInfo *> &Quad::ClassInfo::GetDeclaredPropertyVector() const
{

    return mPropertyVector;
}

void Quad::ClassInfo::SetParentClassList(const char *parentClassNameList, size_t num)
{

    if (num == 0)
        return;

    int strIndex = 0;
    for (size_t classIndex = 0; classIndex < num; ++classIndex)
    {
        std::string className;

        while (1)
        {
            if (parentClassNameList[strIndex] == ' ' || parentClassNameList[strIndex] == '\n')
            {

                mParentClassList.push_back(className);
                strIndex++;
                break;
            }
            else
            {
                className += parentClassNameList[strIndex];
            }

            strIndex++;
        }
    }
}

const std::vector<std::string> &Quad::ClassInfo::GetParentClassList() const
{

    return mParentClassList;
}

void Quad::ClassInfo::SetAncestorClassList(const char *ancestorList, size_t num)
{

    if (num == 0)
        return;

    int strIndex = 0;
    for (size_t classIndex = 0; classIndex < num; ++classIndex)
    {
        std::string className;

        while (1)
        {
            if (ancestorList[strIndex] == ' ' || ancestorList[strIndex] == '\n')
            {

                mAncestorClassList.push_back(className);
                strIndex++;
                break;
            }
            else
            {
                className += ancestorList[strIndex];
            }

            strIndex++;
        }
    }
}
bool Quad::ClassInfo::IsAncestorClass(const char *ancestorClassName) const
{

    std::vector<std::string>::const_iterator it =
        std::find(mAncestorClassList.begin(), mAncestorClassList.end(), ancestorClassName);

    return it == mAncestorClassList.cend() ? false : true;
}

void Quad::ClassInfo::SetAlignment(size_t alignment)
{
    mAlignment = alignment;
}

size_t Quad::ClassInfo::GetAlignment() const
{
    return mAlignment;
}

void Quad::ClassInfo::SetParentClassInfo(ClassInfo *info)
{

    mParentClassInfo = info;
}
Quad::ClassInfo *Quad::ClassInfo::GetParentClassInfo() const
{

    return mParentClassInfo;
}

void Quad::ClassInfo::SetEngineClass(bool flag)
{

    mIsEngineClass = flag;
}

bool Quad::ClassInfo::IsEngineClass() const
{

    return mIsEngineClass;
}