#pragma once
#include <string>
#include <vector>

enum class ETypeCategory
{
    eCommon = 0,
    eStruct,
    eClass,
    ePointer,
    eTypePtr,
    eString,
    eUniqueID
};

enum class EClassType
{
    eCustom = 0,
    eVector
};

struct MetaTag
{
    // 현재 상황에서
    // Component * 의 경우, true라면 단순참조로 처리한다는의미
    bool mIsRef = false;
};

struct ReflectedProperty
{
    std::string mName;
    std::string mType;
    uint64_t mOffset;
    uint64_t mSize;
    std::string mOriginalTypeName; // 포인터타입일경우 역참조되는 타입이름,포인터타입이 아니라면 mType가동일
    bool mIsBuiltinType;
    bool mIsPointerType;
    bool mIsClassType;            // 포인터타입이 true라면 그 역참조하는 타입의 여부
    bool mIsTemplateType = false; // 템플릿타입여부
    bool mIsTemplateElementBuiltinType = false;
    ETypeCategory mTypeCategory;

    std::string mTemplateTypeName = "";                         // 템플릿 타입이름
    std::vector<std::string> mElementTypeNameList;              // 템플릿타입일경우 원소타입이름
    std::vector<std::string> mElementTypeNameWithNamespaceList; // 템플릿타입일경우 원소타입이름
    std::vector<bool> mIsElementClassTypeList;                  // 템플릿타입일경우 원소타입들의 클래스타입여부
    std::vector<ETypeCategory> mElementTypeCategoryList;        // 템플릿
    std::vector<bool> mIsElementPonterTypeList;

    MetaTag mMetaTag;
};

struct classMetaTag
{
    bool mIsEngineClass = false;
};

struct ReflectedClass
{
    EClassType mClassType;
    std::string mName;
    std::string mNameWithinNameSpace;
    bool mIsAbstarct;
    std::vector<ReflectedProperty> mProperties;
    std::vector<std::string> mParentClassNameList;
    std::vector<std::string> mAncestorClassNameList;
    std::vector<std::string> mBaseClassNameList;
    int mBaseClassNameListNum;
    size_t mClassSize;

    classMetaTag mMetaTag;
};