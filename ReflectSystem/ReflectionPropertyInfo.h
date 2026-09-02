#pragma once
#include "ReflectSystem/ReflectSystemDllMacro.h"

class TypeDescriptor;

namespace Quad
{
struct MetaTag
{
    bool mIsReference = false;
};

struct REFLECT_SYSTEM_API PropertyInfo
{
    PropertyInfo() {};

    PropertyInfo(const char *propertyname, const char *t, size_t offset, bool isBuiltinType,
                 const char *originalTypeName, bool IsPointerType, bool isTemplateType, const char *templateTypeName,
                 const char *templateElementTypeName, bool isTemplateElementBuiltinType, const char *TypeDescriptorName,
                 const MetaTag &metaTag)
        : mPropertyName(propertyname), mType(t), mOffset(offset), mIsBuiltinType(isBuiltinType),
          mOriginType(originalTypeName), mIsPointerType(IsPointerType), mIsTemplateType(isTemplateType),
          mTemplateTypeName(templateTypeName), mTemplateElementTypeName(templateElementTypeName),
          mTypeDescriptorName(TypeDescriptorName), mIsTemplateElementBuiltinType(isTemplateElementBuiltinType),
          mTypeDescriptor(nullptr), mMetaTag(metaTag)
    {
    }

    PropertyInfo(const char *propertyname, const char *t, size_t offset, bool isBuiltinType,
                 const char *originalTypeName, bool IsPointerType, bool isTemplateType, const char *templateTypeName,
                 const char *templateElementTypeName, bool isTemplateElementBuiltinType, TypeDescriptor *typeDescriptor,
                 const MetaTag &metaTag)
        : mPropertyName(propertyname), mType(t), mOffset(offset), mIsBuiltinType(isBuiltinType),
          mOriginType(originalTypeName), mIsPointerType(IsPointerType), mIsTemplateType(isTemplateType),
          mTemplateTypeName(templateTypeName), mTemplateElementTypeName(templateElementTypeName),
          mTypeDescriptor(typeDescriptor), mIsTemplateElementBuiltinType(isTemplateElementBuiltinType),
          mMetaTag(metaTag)
    {
    }

    // 기본타입일때만 정상 작동 (int, float ,bool등등 ,+  string ,Vector1, 2,3,4까지허용)
    template <typename T> T GetValue(void *instance) const
    {
        if (instance == nullptr)
            return T{};

        char *pMem = (char *)instance + mOffset;
        return *(reinterpret_cast<T *>(pMem));
    }

    template <typename T> void SetValue(void *instance, const T &value)
    {
        if (instance == nullptr)
            return; //

        char *pMem = (char *)instance + mOffset;
        *(reinterpret_cast<T *>(pMem)) = value;
    }

    template <typename T> T &GetRefValue(void *instance)
    {

        char *pMem = (char *)instance + mOffset;
        return *(reinterpret_cast<T *>(pMem));
    }

    void *GetMemoryStart(void *instance) const
    {
        char *pMem = (char *)instance + mOffset;
        return (void *)pMem;
    }

    const char *mPropertyName = "";
    const char *mType = "";
    const char *mOriginType = ""; // 원본타입, 포인터타입일 경우유효
    size_t mOffset = 0;
    bool mIsPointerType = false;
    bool mIsTemplateType = false;
    const char *mTemplateTypeName = "";
    const char *mTemplateElementTypeName = "";
    TypeDescriptor *mTypeDescriptor = nullptr;
    const char *mTypeDescriptorName = "";
    bool mIsBuiltinType = false;
    bool mIsTemplateElementBuiltinType = false;

    MetaTag mMetaTag;
};

} // namespace Quad