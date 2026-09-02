#pragma once

#include "ReflectType.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <string>
#include <vector>
namespace Quad
{

class IntermediateASTVisitor : public clang::RecursiveASTVisitor<IntermediateASTVisitor>
{
  public:
    std::vector<ReflectedClass> reflectedClasses;

    // 클래스나 구조체를 발견할 때마다 이 함수가 자동으로 호출됩니다.
    bool VisitCXXRecordDecl(clang::CXXRecordDecl *Declaration);

  private:
    void ExtractClassInfo(clang::CXXRecordDecl *Declaration, ReflectedClass &oClass);
    void ExtractPropertyInfo(clang::CXXRecordDecl *Declaration, ReflectedClass &oClass);

    void FindAncestorClass(clang::CXXRecordDecl *Declaration, std::vector<std::string> &oBaseClassList,
                           std::vector<std::string> &oAncestorClassList);

    std::string RemoveScopeFromType(clang::QualType qt, const clang::PrintingPolicy &Policy);

    void BuildMetaTags(MetaTag &metaTag, const std::vector<std::string> &metaTagStrList);

    std::vector<std::string> ExtractMetaTags(const std::string &str);
    void BuildMetaTags(classMetaTag &metaTag, const std::vector<std::string> &metaTagStrList);
};

} // namespace Quad