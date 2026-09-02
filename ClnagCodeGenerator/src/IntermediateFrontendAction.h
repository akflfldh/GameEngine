#pragma once
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include <string>
#include <vector>
namespace Quad
{

class IntermediateFrontendAction : public clang::ASTFrontendAction
{
  public:
    explicit IntermediateFrontendAction(std::vector<std::string> &Out);

    bool BeginSourceFileAction(clang::CompilerInstance &CI) override;
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override;

  private:
    bool CollectIncludeFIles(clang::CompilerInstance &CI);
    std::vector<std::string> &Out; // 외부 벡터를 참조로 보관

    llvm::StringSet<> Uniq;
};

} // namespace Quad