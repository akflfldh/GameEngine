#include "ReflectSystem/ReflectionPropertyInfo.h"
#include <CoreAsset/AssetPtr.h>
#include <CoreBase/Arch.h>
#include <CoreBase/BaseClass.h>
#include <CoreBase/TypeDescriptor.h>
#include <ReflectSystem/ReflectionClassInfo.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <Utility/UniqueID.h>
#include <cassert>

TypeDescriptor_BaseClass::TypeDescriptor_BaseClass() {}

TypeDescriptor_BaseClass::~TypeDescriptor_BaseClass() {}

void TypeDescriptor_BaseClass::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();

    BaseClass *base = static_cast<BaseClass *>(data);
    // void *trueStartAddr = dynamic_cast<void *>(base);
    std::string className = "";
    if (base == nullptr)
    {
    }
    else
    {
        className = base->GetRunTimeClassName();
    }

    arch.StartTable(className);

    if (arch.GetLoadingFlag())
    {

        if (base == nullptr)
        {
            arch.EndTable();
            return;
        }

        Quad::ClassInfo *classInfo = reflectionSystem->FindClassInfo(className.c_str());

        if (classInfo == nullptr)
        {
            arch.EndTable();
            return;
        }
        // 기본적으로 내장된 직렬화(엔진클래스까지만 작업수행)
        base->Serialize(arch);

        while (1)
        {
            std::string propertyName;
            std::string propertyType;
            uint32_t propertyValueSize = 0;

            // 파일에서 하나씩 프로퍼티를 읽어온다.

            arch.ReadPropertyHeader(propertyName, propertyType, propertyValueSize);

            if (propertyName == "None" && propertyType == "None")
                break;

            // 존재하는가 리플렉션시스템에게 묻는다.

            // TODO 수정필요 모든 상속계통에서 프로퍼티를 탐색해야함.
            // 지금은 딱 그 클래스에대한 프로퍼티만 GET 해서 비교고있음
            const Quad::PropertyInfo *propertyInfo = classInfo->GetPropertyInfo(propertyName.c_str());
            if (propertyInfo && (propertyType == propertyInfo->mType))
            {
                // 존재한다면
                /*
                    리플렉션시스템에게 메모리 offset을 반영한 주소를 얻는다.

                    typedescriptor->Serialize(mem) 수행
                    */

                char *pMem = (char *)data + propertyInfo->mOffset;

                // 멤버변수가 Object Pointer, Component Pointer 이면 TypeDescriptor_Pointer로 처리
                if (TypeDescriptor_Pointer *pointerDescriptor =
                        dynamic_cast<TypeDescriptor_Pointer *>(propertyInfo->mTypeDescriptor))
                {
                    pointerDescriptor->Serialize(arch, pMem, const_cast<Quad::PropertyInfo *>(propertyInfo));
                }
                else if (TypeDescriptor_Struct *structDescriptor =
                             dynamic_cast<TypeDescriptor_Struct *>(propertyInfo->mTypeDescriptor))
                {
                    structDescriptor->Serialize(arch, pMem, const_cast<Quad::PropertyInfo *>(propertyInfo));
                }
                else
                {
                    // 그외에 일반적인 포인터
                    propertyInfo->mTypeDescriptor->Serialize(arch, pMem, nullptr);
                }
            }
            else
            {
                // 존재하지않는다면
                /*
                    즉 변수가 제거됬거나 ,새로추가된 변수인것

                    값의 크기만 스킵

                */
                arch.SkipProperty(propertyValueSize);
            }
        }
    }
    else
    {
        Quad::ClassInfo *classInfo = reflectionSystem->FindClassInfo(className.c_str());
        // 기본적으로 내장된 직렬화(엔진클래스까지만 작업수행)
        base->Serialize(arch);
        Serialize_Recursive(arch, data, classInfo);

        // 3. [필수] 저장의 끝을 알리는 더미 태그 기록!
        arch.WritePropertyHeader("None", "None");
        arch.EndProperty();
    }

    arch.EndTable();
}

void TypeDescriptor_BaseClass::Serialize_Recursive(Arch &arch, void *data, Quad::ClassInfo *classInfo)
{

    if ((classInfo == nullptr) || classInfo->IsEngineClass())
        return;

    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();

    // 상속구조
    // 조상들에대해서 재귀적으로 호출

    // TODO 엔진클래스 전까지만 수행해야함
    for (const std::string &parent : classInfo->GetParentClassList())
    {
        Quad::ClassInfo *parentClassInfo = reflectionSystem->FindClassInfo(parent.c_str());
        Serialize_Recursive(arch, data, parentClassInfo);
    }

    for (auto propertyInfo : classInfo->GetDeclaredPropertyVector())
    {
        /* std::string propertyName = propertyInfo->mPropertyName;
         std::string propertyType = propertyInfo->mType;*/
        arch.WritePropertyHeader(propertyInfo->mPropertyName, propertyInfo->mType);

        char *pMem = (char *)data + propertyInfo->mOffset;

        if (TypeDescriptor_Pointer *pointerDescriptor =
                dynamic_cast<TypeDescriptor_Pointer *>(propertyInfo->mTypeDescriptor))
        {
            pointerDescriptor->Serialize(arch, pMem, const_cast<Quad::PropertyInfo *>(propertyInfo));
        }
        else if (TypeDescriptor_Struct *structDescriptor =
                     dynamic_cast<TypeDescriptor_Struct *>(propertyInfo->mTypeDescriptor))
        {
            structDescriptor->Serialize(arch, pMem, const_cast<Quad::PropertyInfo *>(propertyInfo));
        }
        else
        {
            // 그외에 일반적인 포인터
            propertyInfo->mTypeDescriptor->Serialize(arch, pMem);
        }

        arch.EndProperty();
    }
}

void TypeDescriptor_Pointer::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{

    // Component * 인지

    // Object * 인지 구분

    // 만약 기본타입 포인터이다? -> 무시 넘어간다.
    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();
    const std::string &originalTypeStr = propertyInfo->mOriginType;
    Quad::ClassInfo *propertyClassInfo = reflectionSystem->FindClassInfo(originalTypeStr.c_str());

    if (propertyClassInfo == nullptr)
    {
        // 등록되지않은 클래스타입- > 즉 기본타입일것이다.
        return;
    }

    // 클래스타입이다.
    bool bIsComponentType = propertyClassInfo->IsAncestorClass("Component");
    bool bIsObjectType = propertyClassInfo->IsAncestorClass("Object");

    if (bIsComponentType == false && bIsObjectType == false)
    {
        // Componenet타입계열도 Object타입계열도 아니다.
        // 허용되지않은 타입임으로 무시한다.
        return;
    }

    BaseClass *&pBaseClassInstance = *(BaseClass **)data;

    // Object, Component 일떄도 공통적으로는 Name , Type은 기록
    if (arch.GetLoadingFlag())
    {
        bool isValid = true;
        arch << isValid;

        if (isValid)
        {

            if (bIsComponentType)
            {
                if (propertyInfo->mMetaTag.mIsReference)
                {
                    // 참조 메타태그가 설정되었다면
                    // ID로 저장되었기에 ID로 로드한다.
                    CoreUtility::UniqueID componentID;
                    arch << QUAD_SERIALIZEBUFFER(componentID.mUniqueID, 16);

                    reflectionSystem->RequestComponentPointerFix(componentID, data);
                }
                else
                {

                    reflectionSystem->SerializeBaseClass(arch, pBaseClassInstance);
                }
            }
            else if (bIsObjectType)
            {
                // Object타입 계열이다.

                // Object의 ID만 저장한다.
                CoreUtility::UniqueID objectID;
                arch << QUAD_SERIALIZEBUFFER(objectID.mUniqueID, 16);

                // refectionSystem-> requestobjectfix()
                reflectionSystem->RequestObjectPointerFix(objectID, data);
            }
        }
        else
        {
            pBaseClassInstance = nullptr;
        }
    }
    else
    {
        bool isValid = true;
        if (pBaseClassInstance == nullptr)
        {
            isValid = false;
        }

        arch << isValid;

        // 저장
        if (bIsComponentType)
        {
            if (isValid)
            {
                if (propertyInfo->mMetaTag.mIsReference)
                {
                    // 참조 메타태그가 설정되었다면
                    // ID로 저장한다.

                    void *t = *(void **)data;
                    CoreUtility::UniqueID comID = reflectionSystem->GetComponentID(*(void **)data);
                    arch << QUAD_SERIALIZEBUFFER(comID.mUniqueID, 16);
                }
                else
                {

                    reflectionSystem->SerializeBaseClass(arch, pBaseClassInstance);
                }
            }
            //// Component타입계열이다.
        }
        else if (bIsObjectType)
        {
            // Object타입 계열이다.

            // Object의 ID만 저장한다.

            // Object타입 계열이다.
            if (isValid)
            {
                CoreUtility::UniqueID objectID = reflectionSystem->GetObjectID(*(void **)data);
                arch << QUAD_SERIALIZEBUFFER(objectID.mUniqueID, 16);
            }
        }
    }
}

void TypeDescriptor_Struct::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();

    Quad::ClassInfo *classInfo = reflectionSystem->FindClassInfo(propertyInfo->mType);

    std::string typeName = propertyInfo->mType;
    arch.StartTable(typeName);

    if (arch.GetLoadingFlag())
    {
        while (1)
        {
            std::string propertyName;
            std::string propertyType;
            uint32_t propertyValueSize = 0;

            // 파일에서 하나씩 프로퍼티를 읽어온다.
            arch.ReadPropertyHeader(propertyName, propertyType, propertyValueSize);

            if (propertyName == "None" && propertyType == "None")
                break;

            // 존재하는가 리플렉션시스템에게 묻는다.

            // TODO 수정필요 모든 상속계통에서 프로퍼티를 탐색해야함.
            // 지금은 딱 그 클래스에대한 프로퍼티만 GET 해서 비교고있음
            const Quad::PropertyInfo *propertyInfo = classInfo->GetPropertyInfo(propertyName.c_str());
            if (propertyInfo && (propertyType == propertyInfo->mType))
            {
                // 존재한다면
                /*
                    리플렉션시스템에게 메모리 offset을 반영한 주소를 얻는다.

                    typedescriptor->Serialize(mem) 수행
                    */

                char *pMem = (char *)data + propertyInfo->mOffset;

                // 멤버변수가 Object Pointer, Component Pointer 이면 TypeDescriptor_Pointer로 처리
                if (TypeDescriptor_Pointer *pointerDescriptor =
                        dynamic_cast<TypeDescriptor_Pointer *>(propertyInfo->mTypeDescriptor))
                {
                    pointerDescriptor->Serialize(arch, pMem, const_cast<Quad::PropertyInfo *>(propertyInfo));
                }
                else if (TypeDescriptor_Struct *structDescriptor =
                             dynamic_cast<TypeDescriptor_Struct *>(propertyInfo->mTypeDescriptor))
                {
                    structDescriptor->Serialize(arch, pMem, const_cast<Quad::PropertyInfo *>(propertyInfo));
                }
                else
                {
                    // 그외에 일반적인 포인터
                    propertyInfo->mTypeDescriptor->Serialize(arch, pMem);
                }
            }
            else
            {
                // 존재하지않는다면
                /*
                    즉 변수가 제거됬거나 ,새로추가된 변수인것

                    값의 크기만 스킵

                */
                arch.SkipProperty(propertyValueSize);
            }
        }
    }
    else
    {

        for (auto propertyInfo : classInfo->GetDeclaredPropertyVector())
        {
            /* std::string propertyName = propertyInfo->mPropertyName;
             std::string propertyType = propertyInfo->mType;*/
            arch.WritePropertyHeader(propertyInfo->mPropertyName, propertyInfo->mType);

            char *pMem = (char *)data + propertyInfo->mOffset;

            if (TypeDescriptor_Pointer *pointerDescriptor =
                    dynamic_cast<TypeDescriptor_Pointer *>(propertyInfo->mTypeDescriptor))
            {
                pointerDescriptor->Serialize(arch, pMem, const_cast<Quad::PropertyInfo *>(propertyInfo));
            }
            else if (TypeDescriptor_Struct *structDescriptor =
                         dynamic_cast<TypeDescriptor_Struct *>(propertyInfo->mTypeDescriptor))
            {
                structDescriptor->Serialize(arch, pMem, const_cast<Quad::PropertyInfo *>(propertyInfo));
            }
            else
            {
                // 그외에 일반적인 포인터
                propertyInfo->mTypeDescriptor->Serialize(arch, pMem);
            }

            arch.EndProperty();
        }

        // 3. [필수] 저장이 끝났음을 알리는 더미 태그 기록 (BaseClass와 동일)
        arch.WritePropertyHeader("None", "None");
        arch.EndProperty();
    }

    arch.EndTable();
    // 로드

    // 저장

    // 프로퍼티에서 멤버변수들 가져와서 각각의 typeDescriptor의 serialize반복한다.
}

void TypeDescriptor_String::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    std::string *str = static_cast<std::string *>(data);
    arch << *str;
}

void TypeDescriptor_String::Copy(void *source, void *dest)
{

    std::string *sourceStr = static_cast<std::string *>(source);
    std::string *destStr = static_cast<std::string *>(dest);

    *destStr = *sourceStr;
}

void TypeDescriptor_UniqueID::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{

    CoreUtility::UniqueID *id = static_cast<CoreUtility::UniqueID *>(data);
    arch << QUAD_SERIALIZEBUFFER(id->mUniqueID, 16);
}

// void TypeDescriptor_AssetPtr::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
//{
//
//     // 클래스정보 GET
//
//     // ASSET ID  - > 정수
//
//     // 저장 로드 .
//
//     CoreAsset::AssetPtr *assetPtr = (CoreAsset::AssetPtr *)(data);
//
//     std::string typeName = propertyInfo->mType;
//     arch.StartTable(typeName);
//
//     if (arch.GetLoadingFlag())
//     {
//
//         std::string propertyName;
//         std::string propertyType;
//         uint32_t propertyValueSize = 0;
//         arch.ReadPropertyHeader(propertyName, propertyType, propertyValueSize);
//
//         if (propertyName == "AssetID" && propertyType == "AssetID")
//         {
//             CoreAsset::AssetID id = 0;
//             arch << id;
//
//             assetPtr->SetAsset(id);
//         }
//         arch.ReadPropertyHeader(propertyName, propertyType, propertyValueSize);
//     }
//     else
//     {
//         // 어차피 루프 한번
//
//         std::string propertyName;
//         std::string propertyType;
//         uint32_t propertyValueSize = 0;
//
//         arch.WritePropertyHeader("AssetID", "AssetID");
//         CoreAsset::AssetID id = assetPtr->GetAssetID();
//         arch << id;
//         arch.EndProperty();
//
//         arch.WritePropertyHeader("None", "None");
//         arch.EndProperty();
//     }
//
//     arch.EndTable();
// }
Quad::ReflectionSystem *Quad::ReflectionSystem::GetInstance()
{
    static ReflectionSystem instance;
    return &instance;
}

Quad::ReflectionSystem::ReflectionSystem()
{

    // 기본형
    mTypeDescriptorTable["char"] = std::make_unique<TypeDescriptor_char>();
    mTypeDescriptorTable["unsigned char"] = std::make_unique<TypeDescriptor_unsigned_char>();
    mTypeDescriptorTable["short"] = std::make_unique<TypeDescriptor_short>();
    mTypeDescriptorTable["unsigned short"] = std::make_unique<TypeDescriptor_unsigned_short>();
    mTypeDescriptorTable["int"] = std::make_unique<TypeDescriptor_int>();
    mTypeDescriptorTable["unsigned int"] = std::make_unique<TypeDescriptor_unsigned_int>();
    mTypeDescriptorTable["long"] = std::make_unique<TypeDescriptor_long>();
    mTypeDescriptorTable["unsigned long"] = std::make_unique<TypeDescriptor_unsigned_long>();
    mTypeDescriptorTable["long long"] = std::make_unique<TypeDescriptor_long_long>();
    mTypeDescriptorTable["unsigned long long"] = std::make_unique<TypeDescriptor_unsigned_long_long>();
    mTypeDescriptorTable["float"] = std::make_unique<TypeDescriptor_float>();
    mTypeDescriptorTable["double"] = std::make_unique<TypeDescriptor_double>();
    mTypeDescriptorTable["class"] = std::make_unique<TypeDescriptor_BaseClass>();
    mTypeDescriptorTable["pointer"] = std::make_unique<TypeDescriptor_Pointer>();
    mTypeDescriptorTable["bool"] = std::make_unique<TypeDescriptor_bool>();
    mTypeDescriptorTable["string"] = std::make_unique<TypeDescriptor_String>();
    mTypeDescriptorTable["uniqueID"] = std::make_unique<TypeDescriptor_UniqueID>();

    mTypeDescriptorTable["struct"] = std::make_unique<TypeDescriptor_Struct>();

    // STL vector 버전 (요소 타입 참조 전달)
    /*   mTypeDescriptorTable["vector<char>"] =
           std::make_unique<TypeDescriptor_Vector<char>>(mTypeDescriptorTable["char"].get());
       mTypeDescriptorTable["vector<unsigned char>"] =
           std::make_unique<TypeDescriptor_Vector<unsigned char>>(mTypeDescriptorTable["unsigned char"].get());
       mTypeDescriptorTable["vector<short>"] =
           std::make_unique<TypeDescriptor_Vector<short>>(mTypeDescriptorTable["short"].get());
       mTypeDescriptorTable["vector<unsigned short>"] =
           std::make_unique<TypeDescriptor_Vector<unsigned short>>(mTypeDescriptorTable["unsigned short"].get());
       mTypeDescriptorTable["vector<int>"] =
           std::make_unique<TypeDescriptor_Vector<int>>(mTypeDescriptorTable["int"].get());
       mTypeDescriptorTable["vector<unsigned int>"] =
           std::make_unique<TypeDescriptor_Vector<unsigned int>>(mTypeDescriptorTable["unsigned int"].get());
       mTypeDescriptorTable["vector<long>"] =
           std::make_unique<TypeDescriptor_Vector<long>>(mTypeDescriptorTable["long"].get());
       mTypeDescriptorTable["vector<unsigned long>"] =
           std::make_unique<TypeDescriptor_Vector<unsigned long>>(mTypeDescriptorTable["unsigned long"].get());
       mTypeDescriptorTable["vector<long long>"] =
           std::make_unique<TypeDescriptor_Vector<long long>>(mTypeDescriptorTable["long long"].get());
       mTypeDescriptorTable["vector<unsigned long long>"] =
           std::make_unique<TypeDescriptor_Vector<unsigned long long>>(mTypeDescriptorTable["unsigned long
       long"].get()); mTypeDescriptorTable["vector<float>"] =
           std::make_unique<TypeDescriptor_Vector<float>>(mTypeDescriptorTable["float"].get());
       mTypeDescriptorTable["vector<double>"] =
           std::make_unique<TypeDescriptor_Vector<double>>(mTypeDescriptorTable["double"].get());
       mTypeDescriptorTable["vector<class>"] =
           std::make_unique<TypeDescriptor_Vector<class BaseClass *>>(mTypeDescriptorTable["class"].get());*/
}

Quad::ReflectionSystem::~ReflectionSystem() {}

void Quad::ReflectionSystem::Initialize()
{

    ResolveClassParentLinking();
    ResolvePropertyTypeDescriptor();
}

void Quad::ReflectionSystem::ResolveClassParentLinking()
{

    for (auto classInfo : mClassInfoVector)
    {
        const auto &parentClassList = classInfo->GetParentClassList();
        ClassInfo *parentClassInfo = nullptr;
        if (parentClassList.size() != 0)
        {
            const std::string &parentName = parentClassList[0];
            auto it = mClassInfoTable.find(parentName);
            if (it != mClassInfoTable.end())
            {
                parentClassInfo = it->second;
            }
        }

        classInfo->SetParentClassInfo(parentClassInfo);
    }
}

void Quad::ReflectionSystem::ResolvePropertyTypeDescriptor()
{

    for (auto classInfo : mClassInfoVector)
    {
        for (auto property : classInfo->GetDeclaredPropertyVector())
        {
            if (property->mTypeDescriptor == nullptr)
            {
                property->mTypeDescriptor = GetTypeDescriptor(property->mTypeDescriptorName);
            }
        }
    }
}

void Quad::ReflectionSystem::AddProperty(ClassInfo *classInfo, const PropertyInfo &propertyInfo)
{

    PropertyInfo *pPropertyInfo = new PropertyInfo(propertyInfo);

    classInfo->AddProperty(pPropertyInfo);
}

Quad::ClassInfo *Quad::ReflectionSystem::CreateClassInfo(const char *className, size_t classSize, void *(*ctor)(void *),
                                                         void (*dtor)(void *))
{

    ClassInfo *classInfo = new ClassInfo(className, classSize, ctor, dtor);

    mClassInfoVector.push_back(classInfo);
    mClassInfoTable[className] = classInfo;

    return classInfo;
}

BaseClass *Quad::ReflectionSystem::CreateClassInstance(const char *className)
{

    std::unordered_map<std::string, ClassInfo *>::iterator it = mClassInfoTable.find(className);

    if (it == mClassInfoTable.end())
        return nullptr;

    ClassInfo *classInfo = it->second;

    size_t classSize = classInfo->GetClassSize();
    size_t alignment = classInfo->GetAlignment();

    void *mem = mObjectMemoryAllocCallback(classSize, alignment);

    return (BaseClass *)(classInfo->CreateInstance(mem));
}

void Quad::ReflectionSystem::DestoryClassInstance(void *instance)
{

    BaseClass *baseClass = static_cast<BaseClass *>(instance);

    const char *className = baseClass->GetRunTimeClassName();

    std::unordered_map<std::string, ClassInfo *>::iterator it = mClassInfoTable.find(className);

    if (it == mClassInfoTable.end())
        return;

    ClassInfo *classInfo = it->second;

    classInfo->DestoryInstnace(instance);

    // 메모리해제
    // 메모리 해제 하는 콜백도 등록하도록해야할거같다.
    mObjectMemorReleaseCallback(instance);
}

size_t Quad::ReflectionSystem::GetClassSize(const char *className) const
{

    std::unordered_map<std::string, ClassInfo *>::const_iterator it = mClassInfoTable.find(className);

    if (it == mClassInfoTable.end())
        return 0;

    ClassInfo *classInfo = it->second;

    return classInfo->GetClassSize();
}

Quad::ClassInfo *Quad::ReflectionSystem::FindClassInfo(const char *className) const
{

    std::unordered_map<std::string, ClassInfo *>::const_iterator it = mClassInfoTable.find(className);

    if (it == mClassInfoTable.end())
        return nullptr;

    ClassInfo *classInfo = it->second;

    return classInfo;
}

TypeDescriptor *Quad::ReflectionSystem::GetTypeDescriptor(const char *typeName) const
{

    std::unordered_map<std::string, std::unique_ptr<TypeDescriptor>>::const_iterator it =
        mTypeDescriptorTable.find(typeName);
    if (it != mTypeDescriptorTable.cend())
    {
        return it->second.get();
    }

    return nullptr;
}

void Quad::ReflectionSystem::SerializeBaseClass(Arch &arch, BaseClass *&instance)
{

    // TypeDescriptor_baseclass 생성후 serilaize호출할거다.
    TypeDescriptor_BaseClass typeDescriptorBaseClass;
    typeDescriptorBaseClass.Serialize(arch, instance);
}

void Quad::ReflectionSystem::RegisterObjectMemoryAllocCallback(std::function<void *(size_t, size_t)> memCallback)
{

    mObjectMemoryAllocCallback = memCallback;
}

void Quad::ReflectionSystem::RegisterObjectMemoryReleaseCallback(std::function<void(void *)> memCallback)
{

    mObjectMemorReleaseCallback = memCallback;
}

void Quad::ReflectionSystem::RegisterObjectIDGetterCallback(std::function<CoreUtility::UniqueID(void *)> getterCallback)
{

    mObjectIDGetterCallback = getterCallback;
}

void Quad::ReflectionSystem::RegisterObjectGetterCallback(
    std::function<BaseClass *(const CoreUtility::UniqueID &id)> getterCallback)
{

    mObjectGetterCallback = getterCallback;
}

void Quad::ReflectionSystem::RegisterComponentIDGetterCallback(
    std::function<CoreUtility::UniqueID(void *)> getterCallback)
{
    mComponentIDGetterCallback = getterCallback;
}

void Quad::ReflectionSystem::RegisterComponentGetterCallback(
    std::function<BaseClass *(const CoreUtility::UniqueID &)> getterCallback)
{
    mComponentGetterCallback = getterCallback;
}

CoreUtility::UniqueID Quad::ReflectionSystem::GetObjectID(void *ob) const
{

    return mObjectIDGetterCallback(ob);
}

CoreUtility::UniqueID Quad::ReflectionSystem::GetComponentID(void *com) const
{

    return mComponentIDGetterCallback(com);
}

void Quad::ReflectionSystem::RequestObjectPointerFix(CoreUtility::UniqueID id, void *pMem)
{
    mRequestObjectFixQueue.push({id, pMem});
}
void Quad::ReflectionSystem::ProcessObjectPointerFixup()
{

    if (mObjectGetterCallback == nullptr)
        return;

    while (!mRequestObjectFixQueue.empty())
    {

        std::pair<CoreUtility::UniqueID, void *> element = mRequestObjectFixQueue.front();
        mRequestObjectFixQueue.pop();
        void **pointerAddress = static_cast<void **>(element.second);
        BaseClass *foundObject = mObjectGetterCallback(element.first);

        if (foundObject)
        {
            *pointerAddress = foundObject;
        }
    }
}

void Quad::ReflectionSystem::RequestComponentPointerFix(CoreUtility::UniqueID id, void *pMem)
{
    mRequestComponentFixQueue.push({id, pMem});
}
void Quad::ReflectionSystem::ProcessComponentPointerFixup()
{
    if (mComponentGetterCallback == nullptr)
        return;

    while (!mRequestComponentFixQueue.empty())
    {

        std::pair<CoreUtility::UniqueID, void *> element = mRequestComponentFixQueue.front();
        mRequestComponentFixQueue.pop();
        void **pointerAddress = static_cast<void **>(element.second);
        BaseClass *foundComponent = mComponentGetterCallback(element.first);

        if (foundComponent)
        {
            *pointerAddress = foundComponent;
        }
    }
}

void Quad::ReflectionSystem::RegisterTypeDescriptor(const std::string &typeDescriptorName,
                                                    std::unique_ptr<TypeDescriptor> descriptor)
{
    mTypeDescriptorTable[typeDescriptorName] = std::move(descriptor);
}

std::vector<Quad::ClassInfo *> Quad::ReflectionSystem::GetUserClassList() const
{

    std::vector<Quad::ClassInfo *> list;
    for (auto c : mClassInfoVector)
    {
        if (c->IsEngineClass() == false)
        {

            list.push_back(c);
        }
    }

    return list;
}