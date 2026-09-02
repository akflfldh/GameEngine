#include "IntermediateASTConsumer.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h "
#include "llvm/Support/JSON.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>

extern llvm::cl::opt<std::string> OutputJson;
extern llvm::cl::opt<std::string> OutputGPP;
extern llvm::cl::opt<std::string> OutputGH;

void Quad::IntermediateASTConsumer::HandleTranslationUnit(clang::ASTContext &Context)
{

    // Visitor를 이용해 AST 전체 탐사를 시작하라고 명령합니다.
    mVisitor.TraverseDecl(Context.getTranslationUnitDecl());

    llvm::outs() << "[Debug] Reflected class count: " << mVisitor.reflectedClasses.size() << "\n";

    clang::SourceManager &sm = Context.getSourceManager();
    clang::FileID mainFileID = sm.getMainFileID();

    CreateGeneratedCXXFile(Context);

    // CreateIntermediateJsonFile(Context, os);
}

void Quad::IntermediateASTConsumer::CreateGeneratedHeaderFile()
{
    std::error_code ec;
    llvm::SmallString<256> outputPath(OutputGH);
    llvm::raw_fd_ostream os(outputPath, ec, llvm::sys::fs::OpenFlags::OF_Text);

    os << "#pragma once\n";
    os << "#undef GENERATED_BODY\n";
    os << "#define GENERATED_BODY(ClassType) \\\n";
    os << "    friend class ClassInfo; \\\n";
    os << "    friend struct UIElementRegistrar; \\\n";
    os << "public: \\\n";
    os << "    static constexpr const char *GetStaticClassName() { return #ClassType; } \\\n";
    os << "    virtual const char *GetRunTimeClassName() const { return #ClassType; }\n";
}

void Quad::IntermediateASTConsumer::CreateGeneratedCXXFile(clang::ASTContext &Context)
{
    std::error_code ec;
    llvm::SmallString<256> outputPath(OutputGPP);
    llvm::raw_fd_ostream os(outputPath, ec, llvm::sys::fs::OpenFlags::OF_Text);

    if (mVisitor.reflectedClasses.size() == 0)
    {
        // empty json File;
        return;
    }

    // generated.h생성
    CreateGeneratedHeaderFile();

    // 클래스 generated.cpp 생성
    // 프로퍼티가 중요

    // include header 부분생성
    WriteGeneratedCXXHeaderLine(os, Context);
    WriteGeneratedCXXRegisterLine(Context, os);
}

void Quad::IntermediateASTConsumer::CreateIntermediateJsonFile(clang::ASTContext &Context)
{
    std::error_code ec;
    llvm::SmallString<256> outputPath(OutputJson);
    llvm::raw_fd_ostream os(outputPath, ec, llvm::sys::fs::OpenFlags::OF_Text);

    if (mVisitor.reflectedClasses.size() == 0)
    {
        // empty json File;
        return;
    }

    clang::SourceManager &sm = Context.getSourceManager();
    clang::FileID mainFileID = sm.getMainFileID();

    llvm::json::Array classArray;

    for (size_t i = 0; i < mVisitor.reflectedClasses.size(); ++i)
    {
        // 메인파일내에 추출한 클래스 메타데이터를 json 중간파일로 저장한다.
        const ReflectedClass &reflectedClass = mVisitor.reflectedClasses[i];

        llvm::json::Object classObject;

        classObject.insert({"name", reflectedClass.mName});
        classObject.insert({"isAbstract", reflectedClass.mIsAbstarct});

        // top base class name  명시
        llvm::json::Array propertyArray;

        for (const auto &reflectedProperty : reflectedClass.mProperties)
        {
            llvm::json::Object propertyObject;
            propertyObject.insert({"name", reflectedProperty.mName});
            propertyObject.insert({"type", reflectedProperty.mType});
            propertyObject.insert({"offset", static_cast<int64_t>(reflectedProperty.mOffset)});
            propertyObject.insert({"size", reflectedProperty.mSize});
            propertyArray.push_back(std::move(propertyObject));
        }

        classObject.insert({"properties", std::move(propertyArray)});

        classArray.push_back(std::move(classObject));
    }

    // json 파일 생성

    llvm::json::Object root;
    root.insert({"classes", std::move(classArray)});

    if (ec)
    {
        llvm::errs() << "Error opening file: " << ec.message() << "\n";
        return;
    }
    // OutputDir -출력 경로
    //  OutputDir/메인파일name . json 으로 출력
    os << llvm::formatv("{0:2}", llvm::json::Value(std::move(root))); // pretty-print
}

void Quad::IntermediateASTConsumer::WriteGeneratedCXXHeaderLine(llvm::raw_fd_ostream &os, clang::ASTContext &Context)
{

    clang::SourceManager &sm = Context.getSourceManager();
    clang::FileID mainFileID = sm.getMainFileID();

    const clang::FileEntry *mainFile = sm.getFileEntryForID(mainFileID);
    if (mainFile)
    {
        llvm::StringRef mainFileName = llvm::sys::path::filename(mainFile->getName());
        os << "#include" << "\"" << mainFileName << "\"\n";
    }

    os << "#include <ReflectSystem/ReflectionSystem.h>\n";
    os << "#include <ReflectSystem/ReflectionClassInfo.h>\n";
    os << "#include <ReflectSystem/ReflectionPropertyInfo.h>\n";
    os << "#include <cstddef>\n";
}

void Quad::IntermediateASTConsumer::WriteGeneratedCXXRegisterLine(clang::ASTContext &Context, llvm::raw_fd_ostream &os)
{
    for (auto &reflectedClass : mVisitor.reflectedClasses)
    {
        const std::string registerStructName = reflectedClass.mName + "Register";

        os << "struct " + registerStructName << "\n";
        os << "{" << "\n";
        os << "     " << registerStructName << "()" << "\n";
        os << "{" << "\n";
        os << "           Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();" << "\n";

        os << "             Quad::ClassInfo *cls = reflectionSystem->CreateClassInfo(\"" << reflectedClass.mName
           << "\"," << "sizeof(" << reflectedClass.mNameWithinNameSpace << ")";

        if (reflectedClass.mIsAbstarct == false)
        {

            os << ", [](void * mem)->void * {return new(mem) " << reflectedClass.mNameWithinNameSpace << ";},"
               << "[](void * instance){((" << reflectedClass.mNameWithinNameSpace << "*)(instance))->~"
               << reflectedClass.mName << "();});\n";
        }
        else
        {
            // 추상클래스이면 생성자 소멸자 콜백은 nullptr로 등록된다.
            os << ",nullptr, nullptr);\n";
        }

        // 베이스클래스리스트 등록
        os << "             cls->SetBaseClassList(\"";
        for (const auto &base : reflectedClass.mBaseClassNameList)
        {
            os << base << " ";
        }
        os << "\"" << "," << reflectedClass.mBaseClassNameList.size() << ");\n";
        // 부모클래스이름 등록
        os << "             cls->SetParentClassList(\"";
        for (const auto &parent : reflectedClass.mParentClassNameList)
        {
            os << parent << " ";
        }
        os << "\"" << "," << reflectedClass.mParentClassNameList.size() << ");\n";

        // 조상클래스리스트 등록
        os << "             cls->SetAncestorClassList(\"";
        for (const auto &ancestor : reflectedClass.mAncestorClassNameList)
        {
            os << ancestor << " ";
        }
        os << "\"" << "," << reflectedClass.mAncestorClassNameList.size() << ");\n";

        // 정렬값 등록
        os << "             cls->SetAlignment(alignof(" << reflectedClass.mNameWithinNameSpace << "));\n";

        // MetaTag 설정
        WriteClassMetaTags(reflectedClass.mMetaTag, os);

        // property 추가
        ConsumeProperty(reflectedClass.mProperties, os);

        os << "}\n";
        os << "};\n";

        const std::string gRegisterStructInstanceName = "G_" + registerStructName;
        os << "static " << registerStructName << " " << gRegisterStructInstanceName << ";\n";
    }
}

void Quad::IntermediateASTConsumer::WriteClassMetaTags(const classMetaTag &metaTags, llvm::raw_fd_ostream &os)
{
    // EngineClass
    os << "             cls->SetEngineClass(" << ConvertBoolStr(metaTags.mIsEngineClass) << ");\n";
}

void Quad::IntermediateASTConsumer::ConsumeProperty(const std::vector<ReflectedProperty> &propertyList,
                                                    llvm::raw_fd_ostream &os)
{

    for (const auto &reflectedProperty : propertyList)
    {

        // class type이면 typeDescriptor 가 class
        // 그외에는 기본 타입 그대로
        // 근데 포인터이면 ->  참조하는타입을 명시해야한다.
        std::string typeDescriptor = "";
        std::string isPointerType = "false";
        std::string originalType = reflectedProperty.mOriginalTypeName;
        std::string isTemplateType = "false";
        std::string isTemplateElementTypeNameList = "";
        std::string templateElementName = "";
        std::string templateElementNameWithNamespace = "";

        std::string isBuiltinType = reflectedProperty.mIsBuiltinType ? "true" : "false";
        std::string isTemplateElementBuiltinType = reflectedProperty.mIsTemplateElementBuiltinType ? "true" : "false";

        if (reflectedProperty.mIsTemplateType)
        {
            isTemplateType = "true";

            // vector인경우
            std::string templateTypeName = reflectedProperty.mTemplateTypeName;

            if (reflectedProperty.mElementTypeCategoryList.size() == 0)
            {
                typeDescriptor = reflectedProperty.mOriginalTypeName;
            }
            else
            {

                if (templateTypeName == "vector")
                {
                    templateElementName = reflectedProperty.mElementTypeNameList[0];
                    templateElementNameWithNamespace = reflectedProperty.mElementTypeNameWithNamespaceList[0];
                    switch (reflectedProperty.mElementTypeCategoryList[0])
                    {
                    case ETypeCategory::eCommon:
                    {
                        typeDescriptor = templateElementName;
                    }
                    break;
                    case ETypeCategory::eStruct:
                    {
                        typeDescriptor = "struct";
                    }
                    break;
                    case ETypeCategory::eClass:
                    {
                        typeDescriptor = "class";
                    }
                    break;
                    case ETypeCategory::eTypePtr:
                    {
                        typeDescriptor = "AssetPtr";
                    }
                    break;
                    case ETypeCategory::ePointer:
                    {
                        typeDescriptor = "pointer";
                        isPointerType = "true";
                    }
                    break;
                    case ETypeCategory::eString:
                    {
                        typeDescriptor = "string";
                    }

                    break;
                    case ETypeCategory::eUniqueID:
                    {
                        typeDescriptor = "uniqueID";
                    }
                    break;
                    }
                }
            }
        }
        else
        {
            switch (reflectedProperty.mTypeCategory)
            {

            case ETypeCategory::eCommon:
            {
                typeDescriptor = reflectedProperty.mOriginalTypeName;
            }
            break;
            case ETypeCategory::eStruct:
            {
                typeDescriptor = "struct";
            }
            break;
            case ETypeCategory::eClass:
            {
                typeDescriptor = "class";
            }
            break;
            case ETypeCategory::eTypePtr:
            {
                typeDescriptor = "AssetPtr";
            }
            break;
            case ETypeCategory::ePointer:
            {
                typeDescriptor = "pointer";
            }
            break;
            case ETypeCategory::eString:
            {
                typeDescriptor = "string";
            }
            break;
            case ETypeCategory::eUniqueID:
            {
                typeDescriptor = "uniqueID";
            }
            break;
            }

            if (reflectedProperty.mIsPointerType)
            {
                isPointerType = "true";
            }
        }

        WriteGeneratedCXXAddingPropertyLine(
            os, reflectedProperty.mName, reflectedProperty.mType, reflectedProperty.mOffset, isBuiltinType,
            originalType, isPointerType, isTemplateType, reflectedProperty.mTemplateTypeName, templateElementName,
            templateElementNameWithNamespace, isTemplateElementBuiltinType, typeDescriptor,
            reflectedProperty.mTypeCategory, reflectedProperty.mIsTemplateType, reflectedProperty.mMetaTag);
    }
}

void Quad::IntermediateASTConsumer::WriteGeneratedCXXAddingPropertyLineCommmon() {}

void Quad::IntermediateASTConsumer::WriteGeneratedCXXAddingPropertyLine(
    llvm::raw_fd_ostream &os, const std::string &propertyName, const std::string &typeName, size_t offset,
    const std::string &isBuiltinType, const std::string &originalTypeName, const std::string &isPointerType,
    const std::string &isTemplateType, const std::string &templateTypeName, const std::string &templateElementTypeName,
    const std::string &templateElementTypeNameWithNamespace, const std::string &isTemplateElementBuiltinType,
    const std::string &typeDescriptorName, ETypeCategory typeCategory, bool bTemplate, const MetaTag &metaTag)
{

    os << "reflectionSystem->AddProperty(cls,";
    os << "Quad::PropertyInfo{" << "\"" << propertyName << "\"" << ",";
    os << "\"" << typeName << "\"" << ",";
    os << offset << ",";
    os << isBuiltinType << ",";
    os << "\"" << originalTypeName << "\"" << ",";
    os << isPointerType << ",";
    os << isTemplateType << ",";
    os << "\"" << templateTypeName << "\",";
    os << "\"" << templateElementTypeName << "\",";
    os << isTemplateElementBuiltinType << ",";

    if (bTemplate)
    {

        if (templateTypeName == "vector")
        {
            os << " reflectionSystem->GetVectorTypeDescriptor<" << templateElementTypeNameWithNamespace << ">("
               << "\"" << templateElementTypeName << "\"" << "," << "\"" << typeDescriptorName << "\"" << ")";
        }
        else
        {
        }
    }
    else
    {
        if (typeCategory == ETypeCategory::eTypePtr)
        {
            os << "\"" << typeDescriptorName << "\"";
        }
        else
        {

            os << " reflectionSystem->GetTypeDescriptor(" << "\"" << typeDescriptorName << "\""
               << ")";
        }
    }

    // metaTag

    os << ","
       << "Quad::MetaTag{" << ConvertBoolStr(metaTag.mIsRef) << "}";

    // property end
    os << "}); \n";
}

std::string Quad::IntermediateASTConsumer::ConvertBoolStr(bool b)
{
    return b == true ? "true" : "false";
}
