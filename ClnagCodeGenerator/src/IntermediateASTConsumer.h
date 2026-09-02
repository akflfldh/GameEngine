#pragma once

#include "IntermediateASTVisitor.h"
#include "clang/AST/ASTConsumer.h"
#include <string>

namespace Quad
{

class IntermediateASTConsumer : public clang::ASTConsumer
{
  private:
    IntermediateASTVisitor mVisitor;
    std::string TUBaseName; // 이 TU의 기본 베이스명 (예: UIType)

  public:
    // 소스 파일 전체의 AST가 준비되면 이 함수가 호출됩니다.
    void HandleTranslationUnit(clang::ASTContext &Context) override;

  private:
    void CreateGeneratedHeaderFile();
    void CreateGeneratedCXXFile(clang::ASTContext &Context);
    void CreateIntermediateJsonFile(clang::ASTContext &Context);
    void WriteGeneratedCXXHeaderLine(llvm::raw_fd_ostream &os, clang::ASTContext &Context);
    void WriteGeneratedCXXRegisterLine(clang::ASTContext &Context, llvm::raw_fd_ostream &os);
    void WriteClassMetaTags(const classMetaTag & metaTags, llvm::raw_fd_ostream  & os);

    void ConsumeProperty(const std::vector<ReflectedProperty> &propertyList, llvm::raw_fd_ostream &os);

    void WriteGeneratedCXXAddingPropertyLineCommmon();
    void WriteGeneratedCXXAddingPropertyLine(
        llvm::raw_fd_ostream &os, const std::string &propertyName, const std::string &typeName, size_t offset,
        const std::string &isBuiltinType, const std::string &originalTypeName, const std::string &isPointerType,
        const std::string &isTemplateType, const std::string &templateTypeName,
        const std::string &templateElementTpyeName, const std::string &templateElementTypeNameWithNamespace,
        const std::string &isTemplateElementBuiltinType, const std::string &typeDescriptorName,
        ETypeCategory typeCategory, bool bTemplate, const MetaTag &metaTag);

    std::string ConvertBoolStr(bool b);
};
} // namespace Quad