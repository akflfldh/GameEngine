#include "IntermediateASTVisitor.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/RecordLayout.h"
#include "clang/Basic/SourceManager.h"
#include <queue>
#include <set>
#include <sstream>

// 클래스나 구조체를 발견할 때마다 이 함수가 자동으로 호출됩니다.
bool Quad::IntermediateASTVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *Declaration)
{

    // 전방선언 건너뛰기
    if (Declaration->isThisDeclarationADefinition() == false)
        return true;

    // main파일에대한 클래스인지 확인해야한다. 메인파일에서 정의된 클래스만 처리할것이다.
    clang::SourceManager &sm = Declaration->getASTContext().getSourceManager();

    // 메인파일에 정의된 클래스가 아니기에 조기반환
    if (sm.isInMainFile(Declaration->getLocation()) == false)
        return true;

    // 'annotate' 속성(우리가 꽂은 깃발)이 있는지 확인합니다.
    if (Declaration->hasAttr<clang::AnnotateAttr>() == false)
        return true;

    clang::AnnotateAttr *Attr = Declaration->getAttr<clang::AnnotateAttr>();
    // 그 깃발의 내용이 "REFLECT_CLASS"가 맞는지 확인합니다.

    if (Attr == nullptr)
        return true;

    llvm::StringRef stringRef = Attr->getAnnotation();

    if (stringRef.startswith("REFLECT_CLASS") == false)
        return true;

    ReflectedClass rc;
    std::vector<std::string> metaTagsVec = ExtractMetaTags(stringRef.str());
    BuildMetaTags(rc.mMetaTag, metaTagsVec);

    ExtractClassInfo(Declaration, rc);
    ExtractPropertyInfo(Declaration, rc);

    // 찾은 클래스 저장
    reflectedClasses.push_back(rc);

    return true; // 계속해서 다른 노드도 탐색하라는 의미입니다.
}

void Quad::IntermediateASTVisitor::ExtractClassInfo(clang::CXXRecordDecl *Declaration, ReflectedClass &oClass)
{

    oClass.mName = Declaration->getNameAsString();
    oClass.mIsAbstarct = Declaration->isAbstract();

    auto &Ctx = Declaration->getASTContext();
    const clang::ASTRecordLayout &Layout = Ctx.getASTRecordLayout(Declaration);

    // class Size
    clang::CharUnits::QuantityType classSize = Layout.getSize().getQuantity();
    oClass.mClassSize = classSize;

    // 부모클래스 리스트
    for (const auto &base : Declaration->bases())
    {
        const clang::Type *baseType = base.getType().getTypePtr();

        clang::CXXRecordDecl *baseClassDecl = baseType->getAsCXXRecordDecl();

        const std::string baseClassTypeStr = baseClassDecl->getNameAsString();

        oClass.mParentClassNameList.push_back(baseClassTypeStr);
    }
    // 최상위 베이스 클래스 리스트
    std::vector<std::string> topBaseClassNameVec;
    std::vector<std::string> ancenstorClassNameVec;

    FindAncestorClass(Declaration, topBaseClassNameVec, ancenstorClassNameVec);

    oClass.mBaseClassNameList = topBaseClassNameVec;
    oClass.mBaseClassNameListNum = topBaseClassNameVec.size();
    oClass.mAncestorClassNameList = std::move(ancenstorClassNameVec);

    oClass.mNameWithinNameSpace = Declaration->getQualifiedNameAsString();
}

void Quad::IntermediateASTVisitor::ExtractPropertyInfo(clang::CXXRecordDecl *Declaration, ReflectedClass &oClass)
{

    auto &Ctx = Declaration->getASTContext();
    const clang::ASTRecordLayout &Layout = Ctx.getASTRecordLayout(Declaration);

    clang::PrintingPolicy policy(Ctx.getLangOpts());
    policy.Bool = 1;
    policy.SuppressScope = true;
    policy.SuppressTagKeyword = true;

    for (auto *Field : Declaration->fields())
    {
        if (Field->hasAttr<clang::AnnotateAttr>())
        {
            llvm::StringRef stringRef = Field->getAttr<clang::AnnotateAttr>()->getAnnotation();
            // 멤버 변수에도 "REFLECT_PROPERTY" 깃발이 꽂혀 있는지 확인합니다.

            if (stringRef.startswith("REFLECT_PROPERTY"))
            {
                std::vector<std::string> metaTags;

                std::string annotateAttrStr = stringRef.str();

                size_t commaPos = annotateAttrStr.find(',');

                if (commaPos != std::string::npos)
                {
                    // REFLECT_PROPERTY , 를 잘라냄
                    // 순수 매개변수 string
                    std::string paramStr = annotateAttrStr.substr(commaPos + 1);

                    std::istringstream iss(paramStr);
                    std::string token;

                    while (std::getline(iss, token, ','))
                    {
                        // 앞뒤 공백제거
                        token.erase(0, token.find_first_not_of(" \t"));
                        token.erase(token.find_last_not_of(" \t") + 1);

                        metaTags.push_back(token);
                    }
                }

                llvm::outs() << "[DEBUG] Parsing Property: " << Field->getNameAsString() << "\n";

                clang::ASTContext &astContext = Field->getASTContext();

                unsigned idx = Field->getFieldIndex(); // 선언 순서 인덱스
                uint64_t bitOff = Layout.getFieldOffset(idx);
                uint64_t byteOff = bitOff / 8;
                unsigned long valueSize = astContext.getTypeSize(Field->getType()) / 8;

                ReflectedProperty property;

                BuildMetaTags(property.mMetaTag, metaTags);

                property.mName = Field->getNameAsString();
                // property.mType = Field->getType().getAsString(policy);
                clang::QualType qt = Field->getType().getUnqualifiedType();
                std::string rawTypeName = RemoveScopeFromType(qt, policy);

                property.mType = rawTypeName;

                property.mOffset = byteOff;
                property.mSize = valueSize;
                property.mIsPointerType = Field->getType().getTypePtr()->isPointerType();

                const clang::Type *propertyType = property.mIsPointerType
                                                      ? Field->getType().getTypePtr()->getPointeeType().getTypePtr()
                                                      : Field->getType().getTypePtr();

                property.mIsBuiltinType = propertyType->isBuiltinType();

                // 템플릿
                if (auto *tst = Field->getType().getTypePtr()->getAs<clang::TemplateSpecializationType>())
                {
                    if (property.mType != "string")
                    {

                        llvm::outs()
                            << "[DEBUG] Entered TemplateSpecializationType!\n"; // 여기가 안 찍히면 1번 용의자가 범인!
                        property.mIsTemplateType = true;
                        auto *tempDecl = tst->getTemplateName().getAsTemplateDecl();

                        if (tempDecl)
                        {
                            auto &tempAstContext = tempDecl->getASTContext();
                            clang::PrintingPolicy policy(tempAstContext.getLangOpts());
                            // 2. 네임스페이스를 생략하도록 설정합니다.
                            policy.SuppressScope = true;

                            property.mTemplateTypeName = tempDecl->getIdentifier()->getName().str();
                            // 원소 타입문자열
                            for (const auto &arg : tst->template_arguments())
                            {
                                if (arg.getKind() == clang::TemplateArgument::Type)
                                {
                                    clang::QualType argType = arg.getAsType();
                                    const clang::Type *type = argType.getTypePtr();
                                    if (argType.isNull())
                                    {
                                        llvm::errs() << "[Warning] Null QualType found in template argument!\n";
                                        continue; // 무시하고 다음 인자로 넘어갑니다.
                                    }
                                    policy.SuppressScope = false;
                                    std::string elementTypeNameWithNameSpace =
                                        argType.getUnqualifiedType().getAsString(policy);
                                    policy.SuppressScope = true;

                                    // 2 일단 정책을 적용해서 문자열로 뽑아냄
                                    std::string elementTypeName = argType.getUnqualifiedType().getAsString(policy);
                                    property.mIsTemplateElementBuiltinType = type->isBuiltinType();

                                    // 3. 확실한 네임스페이스 제거 (템플릿 인자는 policy가 안 먹힐 때가 많음!)
                                    size_t lastColonPos = elementTypeName.rfind("::");
                                    if (lastColonPos != std::string::npos)
                                    {
                                        elementTypeName = elementTypeName.substr(lastColonPos + 2);
                                    }
                                    size_t lastarrowPos = elementTypeName.rfind(">");
                                    if (lastarrowPos != std::string::npos)
                                    {
                                        elementTypeName = elementTypeName.substr(0, lastarrowPos);
                                    }

                                    property.mElementTypeNameList.push_back(elementTypeName);
                                    property.mElementTypeNameWithNamespaceList.push_back(elementTypeNameWithNameSpace);
                                    ETypeCategory elementTypeCategory;
                                    if (type->isClassType())
                                    {

                                        elementTypeCategory = ETypeCategory::eClass;

                                        if (elementTypeName == "AssetPtr")
                                        {
                                            elementTypeCategory = ETypeCategory::eTypePtr;
                                        }
                                        else if (type->isPointerType())
                                        {
                                            elementTypeCategory = ETypeCategory::ePointer;
                                            property.mIsElementPonterTypeList.push_back(true);
                                        }
                                    }
                                    else if (type->isStructureType())
                                    {
                                        elementTypeCategory = ETypeCategory::eStruct;
                                    }
                                    else
                                    {
                                        elementTypeCategory = ETypeCategory::eCommon;
                                    }

                                    property.mElementTypeCategoryList.push_back(elementTypeCategory);
                                }
                            }
                        }
                    }
                }
                else
                {
                    llvm::outs() << "[DEBUG] Not recognized as TemplateSpecializationType.\n";
                }

                if (propertyType->isClassType())
                {
                    property.mTypeCategory = ETypeCategory::eClass;

                    if (property.mType == "AssetPtr")
                    {
                        property.mTypeCategory = ETypeCategory::eTypePtr;
                    }
                    else if (property.mType == "string")
                    {
                        property.mTypeCategory = ETypeCategory::eString;
                    }
                    else if (property.mType == "UniqueID")
                    {
                        property.mTypeCategory = ETypeCategory::eUniqueID;
                    }
                }
                else if (propertyType->isStructureType())
                {
                    property.mTypeCategory = ETypeCategory::eStruct;
                }
                else
                {
                    property.mTypeCategory = ETypeCategory::eCommon;
                }

                if (property.mIsPointerType)
                {
                    // 역참조하는 객체의 클래스타입여부
                    const clang::Type *orignalType = Field->getType().getTypePtr()->getPointeeType().getTypePtr();
                    property.mIsClassType = orignalType->isClassType();
                    property.mOriginalTypeName = Field->getType().getTypePtr()->getPointeeType().getAsString(policy);
                    property.mTypeCategory = ETypeCategory::ePointer;
                }
                else
                {
                    property.mIsClassType = Field->getType().getTypePtr()->isClassType();
                    property.mOriginalTypeName = property.mType;
                }

                oClass.mProperties.push_back(std::move(property));
            }
        }
    }
}

void Quad::IntermediateASTVisitor::FindAncestorClass(clang::CXXRecordDecl *Declaration,
                                                     std::vector<std::string> &oBaseClassList,
                                                     std::vector<std::string> &oAncestorClassList)
{

    std::vector<std::string> baseClassNameVector;

    std::queue<const clang::CXXRecordDecl *> baseClassQueue;
    std::set<std::string> topBaseClassNameSet;
    std::set<std::string> allAncestorClassSet;

    baseClassQueue.push(Declaration);

    while (!baseClassQueue.empty())
    {
        const clang::CXXRecordDecl *Declaration = baseClassQueue.front();
        baseClassQueue.pop();

        allAncestorClassSet.insert(Declaration->getNameAsString());

        if (Declaration->bases().empty())
        {
            topBaseClassNameSet.insert(Declaration->getNameAsString());
        }
        else
        {
            Declaration->forallBases(
                [&baseClassQueue](const clang::CXXRecordDecl *baseDecl)
                {
                    baseClassQueue.push(baseDecl);
                    return true;
                });
        }
    }

    std::vector<std::string> topBaseClassNameVector(topBaseClassNameSet.begin(), topBaseClassNameSet.end());
    std::vector<std::string> ancestorClassNameVector(allAncestorClassSet.begin(), allAncestorClassSet.end());

    oBaseClassList = std::move(topBaseClassNameVector);
    oAncestorClassList = std::move(ancestorClassNameVector);
}

std::string Quad::IntermediateASTVisitor::RemoveScopeFromType(clang::QualType qt, const clang::PrintingPolicy &Policy)
{
    //// 1. 기존 정책을 복사합니다.
    // clang::PrintingPolicy modifiedPolicy = Policy;

    //// 2. 핵심 설정: 네임스페이스(Scope) 출력을 억제합니다.
    // modifiedPolicy.SuppressScope = true;

    //// (선택) 만약 'class MyClass'가 아니라 'MyClass'만 얻고 싶다면 아래 설정도 켭니다.
    // modifiedPolicy.SuppressTagKeyword = true;

    //// 3. UnqualifiedType으로 만든 후, 수정된 정책을 적용해 문자열로 반환합니다.
    // return qt.getUnqualifiedType().getAsString(modifiedPolicy);

    if (const clang::TypedefType *tdt = qt->getAs<clang::TypedefType>())
    {
        // 'basic_string...'까지 파고들지 않고, 별칭인 'string'만 반환하고 끝냅니다.
        return tdt->getDecl()->getName().str();
    }

    qt = qt.getUnqualifiedType();
    const clang::Type *t = qt.getTypePtr();

    if (const clang::TemplateSpecializationType *tst = qt->getAs<clang::TemplateSpecializationType>())
    {
        std::string name = tst->getTemplateName().getAsTemplateDecl()->getName().str();

        std::string args = "<";
        for (size_t i = 0; i < tst->template_arguments().size(); ++i)
        {
            if (i > 0)
                args += ", ";

            const clang::TemplateArgument &arg = tst->template_arguments()[i];
            if (arg.getKind() == clang::TemplateArgument::ArgKind::Type)
            {
                args += RemoveScopeFromType(arg.getAsType(), Policy); // 재귀
            }
            else
            {
                // 정수값
                llvm::raw_string_ostream tempOs(args);
                arg.print(Policy, tempOs, false);
            }
        }
        args += ">";
        name += args;

        return name;
    }

    if (const clang::TagType *tt = t->getAs<clang::TagType>())
    {
        return tt->getDecl()->getName().str();
    }

    //(int, float 등 기본 타입)
    return qt.getAsString(Policy);
}

void Quad::IntermediateASTVisitor::BuildMetaTags(MetaTag &metaTag, const std::vector<std::string> &metaTagStrList)
{

    auto it = std::find(metaTagStrList.begin(), metaTagStrList.end(), "IsReference");
    if (it != metaTagStrList.end())
    {
        metaTag.mIsRef = true;
    }
}

std::vector<std::string> Quad::IntermediateASTVisitor::ExtractMetaTags(const std::string &str)
{

    size_t commaPos = str.find(',');

    std::vector<std::string> tagvec;
    if (commaPos != std::string::npos)
    {
        std::string paramStr = str.substr(commaPos + 1);

        std::istringstream iss(paramStr);

        std::string token;
        while (std::getline(iss, token, ','))
        {

            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);
            tagvec.push_back(token);
        }
    }

    return tagvec;
}

void Quad::IntermediateASTVisitor::BuildMetaTags(classMetaTag &metaTag, const std::vector<std::string> &metaTagStrList)
{

    auto it = std::find(metaTagStrList.begin(), metaTagStrList.end(), "EngineClass");

    if (it != metaTagStrList.end())
    {
        metaTag.mIsEngineClass = true;
    }
}
