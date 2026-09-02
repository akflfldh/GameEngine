#pragma once

#include "ReflectSystem/ReflectSystemDllMacro.h"
#include <CoreBase/TypeDescriptor.h>
#include <ReflectSystem/ReflectionPropertyInfo.h>
#include <Utility/UniqueID.h>
#include <cstddef>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

namespace Quad
{
class ClassInfo;
class PropertyInfo;

} // namespace Quad

struct REFLECT_SYSTEM_API TypeDescriptor_BaseClass : public TypeDescriptor
{
  public:
    TypeDescriptor_BaseClass();
    virtual ~TypeDescriptor_BaseClass();

    void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr);

  private:
    void Serialize_Recursive(Arch &arch, void *data, Quad::ClassInfo *classInfo);
};

struct REFLECT_SYSTEM_API TypeDescriptor_Pointer : public TypeDescriptor
{
  public:
    void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo) override;
};

struct REFLECT_SYSTEM_API TypeDescriptor_Struct : public TypeDescriptor
{

  public:
    void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo) override;
};

struct REFLECT_SYSTEM_API TypeDescriptor_String : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo) override;
    virtual void Copy(void *source, void *dest) override;
};

struct REFLECT_SYSTEM_API TypeDescriptor_UniqueID : public TypeDescriptor
{
  public:
    void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo) override;
};

// struct REFLECT_SYSTEM_API TypeDescriptor_AssetPtr : public TypeDescriptor
//{
//
//   public:
//     virtual void Serialize(Arch &arch, void *data) {};
//     void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo);
// };

class BaseClass;
template <typename T> class TypeDescriptor_Vector;

namespace Quad
{

class REFLECT_SYSTEM_API ReflectionSystem
{

  public:
    static ReflectionSystem *GetInstance();
    ReflectionSystem();
    ~ReflectionSystem();
    ReflectionSystem(const ReflectionSystem &) = delete;
    ReflectionSystem &operator=(const ReflectionSystem &) = delete;

    void Initialize();
    void ResolveClassParentLinking();
    void ResolvePropertyTypeDescriptor();

    void AddProperty(ClassInfo *classInfo, const PropertyInfo &propertyInfo);

    ClassInfo *CreateClassInfo(const char *ClassName, size_t classSize, void *(*ctor)(void *), void (*dtor)(void *));

    BaseClass *CreateClassInstance(const char *className);
    void DestoryClassInstance(void *instance);

    size_t GetClassSize(const char *className) const;
    ClassInfo *FindClassInfo(const char *className) const;

    TypeDescriptor *GetTypeDescriptor(const char *tpyeName) const;

    // 정확히는 Object
    void SerializeBaseClass(Arch &arch, BaseClass *&instance);

    void RegisterObjectMemoryAllocCallback(std::function<void *(size_t, size_t)> memCallback);
    void RegisterObjectMemoryReleaseCallback(std::function<void(void *)> memCallback);
    void RegisterObjectIDGetterCallback(std::function<CoreUtility::UniqueID(void *)> getterCallback);
    void RegisterObjectGetterCallback(std::function<BaseClass *(const CoreUtility::UniqueID &)> getterCallback);

    void RegisterComponentIDGetterCallback(std::function<CoreUtility::UniqueID(void *)> getterCallback);
    void RegisterComponentGetterCallback(std::function<BaseClass *(const CoreUtility::UniqueID &)> getterCallback);

    CoreUtility::UniqueID GetObjectID(void *ob) const;
    CoreUtility::UniqueID GetComponentID(void *ob) const;

    void RequestObjectPointerFix(CoreUtility::UniqueID id, void *pMem);
    void ProcessObjectPointerFixup();
    void ProcessComponentPointerFixup();

    void RequestComponentPointerFix(CoreUtility::UniqueID id, void *pMem);

    template <typename T>
    TypeDescriptor *GetVectorTypeDescriptor(const std::string &elementTypeName,
                                            const std::string &elementTypeDescriptorName);

    void RegisterTypeDescriptor(const std::string &typeDescriptorName, std::unique_ptr<TypeDescriptor> descriptor);

    std::vector<ClassInfo *> GetUserClassList() const;

  private:
    std::vector<ClassInfo *> mClassInfoVector;
    std::unordered_map<std::string, ClassInfo *> mClassInfoTable;
    std::unordered_map<std::string, std::unique_ptr<TypeDescriptor>> mTypeDescriptorTable;

    std::function<CoreUtility::UniqueID(void *)> mObjectIDGetterCallback;
    std::function<BaseClass *(const CoreUtility::UniqueID &)> mObjectGetterCallback;
    std::function<BaseClass *(const CoreUtility::UniqueID &)> mComponentGetterCallback;

    std::queue<std::pair<CoreUtility::UniqueID, void *>> mRequestObjectFixQueue;
    std::queue<std::pair<CoreUtility::UniqueID, void *>> mRequestComponentFixQueue;

    std::function<void *(size_t size, size_t alignment)> mObjectMemoryAllocCallback;
    std::function<void(void *)> mObjectMemorReleaseCallback;

    // com
    std::function<CoreUtility::UniqueID(void *)> mComponentIDGetterCallback;
};

template <typename T>
TypeDescriptor *ReflectionSystem::GetVectorTypeDescriptor(const std::string &elementTypeName,
                                                          const std::string &elementTypeDescriptorName)
{

    // 테이블 키: "std::vector<AssetPtr>"
    std::string key = "std::vector<" + elementTypeName + ">";
    auto it = mTypeDescriptorTable.find(key);
    if (it != mTypeDescriptorTable.end())
    {
        return it->second.get();
    }

    // 없다면 새로 만들어서 테이블에 등록하고 반환!
    auto newDesc = std::make_unique<TypeDescriptor_Vector<T>>(elementTypeName, elementTypeDescriptorName);
    TypeDescriptor *pResult = newDesc.get();
    mTypeDescriptorTable[key] = std::move(newDesc);

    return pResult;
}

// reflectionSystem->GetVectorTypeDescriptor<CoreAsset::AssetPtr>("AssetPtr", "AssetPtr")

} // namespace Quad

class ITypeDescriptor_Vector : public TypeDescriptor
{
  public:
    virtual size_t GetSize(void *vectorPtr) const = 0;

    virtual void *GetElementPtr(void *vectorPtr, size_t index) const = 0;

    virtual void Resize(void *vectorPtr, size_t newSize) const = 0;
};

template <typename T> class TypeDescriptor_Vector : public ITypeDescriptor_Vector
{
  public:
    TypeDescriptor_Vector(const std::string &elementTypeName, const std::string &elementDescriptorTypeName)
        : mElementTypeName(elementTypeName), mElementDescriptorTypeName(elementDescriptorTypeName)
    {
    }

    // virtual void Serialize(Arch &arch, void *data) {}
    void Serialize(Arch &arch, void *data, Quad::PropertyInfo *info) override
    {
        Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();
        TypeDescriptor *elementDesc = reflectionSystem->GetTypeDescriptor(mElementDescriptorTypeName.c_str());

        std::vector<T> *vec = static_cast<std::vector<T> *>(data);
        std::string typeName = "vec";
        arch.StartTable(typeName);
        if (arch.GetLoadingFlag())
        {

            // 로드

            uint32_t size = 0;
            arch << size;

            vec->resize(size);
            for (uint32_t i = 0; i < size; ++i)
            {
                char *elementMem = (char *)&((*vec)[i]); // 원소의 메모리 주소

                // 구조체 재귀 로직 등을 위해 임시 PropertyInfo 전달
                Quad::PropertyInfo dummyProp;
                dummyProp.mOriginType = mElementTypeName.c_str();

                elementDesc->Serialize(arch, elementMem, &dummyProp);
            }
        }
        else
        {

            uint32_t size = vec->size();
            arch << size;

            for (uint32_t i = 0; i < size; ++i)
            {
                char *elementMem = (char *)&((*vec)[i]); // 원소의 메모리 주소

                // 구조체 재귀 로직 등을 위해 임시 PropertyInfo 전달
                Quad::PropertyInfo dummyProp;
                dummyProp.mOriginType = mElementTypeName.c_str();

                elementDesc->Serialize(arch, elementMem, &dummyProp);
            }

            // 저장
        }
        arch.EndTable();
    }

    virtual size_t GetSize(void *vectorPtr) const override
    {
        std::vector<T> *vec = static_cast<std::vector<T> *>(vectorPtr);
        return vec->size();
    }

    virtual void *GetElementPtr(void *vectorPtr, size_t index) const override
    {
        if (GetSize(vectorPtr) <= index)
            return nullptr;

        std::vector<T> *vec = static_cast<std::vector<T> *>(vectorPtr);

        return static_cast<void *>(&((*vec)[index]));
    }

    virtual void Resize(void *vectorPtr, size_t newSize) const override
    {
        std::vector<T> *vec = static_cast<std::vector<T> *>(vectorPtr);

        (*vec).resize(newSize);
    }

  private:
    std::string mElementTypeName;
    std::string mElementDescriptorTypeName;
};