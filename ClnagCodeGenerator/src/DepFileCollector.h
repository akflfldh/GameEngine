#pragma once
#include "clang/Lex/PPCallbacks.h"
#include "llvm/ADT/StringSet.h"
#include <string>
#include <vector>
namespace Quad
{

class DepFileCollector : public clang::PPCallbacks
{
  public:
    DepFileCollector(clang::SourceManager &SM, llvm::StringSet<> &uniq, std::vector<std::string> &out)
        : SM(SM), Uniq(uniq), Out(out)
    {
    }

    void InclusionDirective(clang::SourceLocation, const clang::Token &, llvm::StringRef /*FileName*/,
                            bool /*IsAngled*/, clang::CharSourceRange /*FilenameRange*/,
                            clang::OptionalFileEntryRef File, llvm::StringRef /*SearchPath*/,
                            llvm::StringRef /*RelativePath*/, const clang::Module * /*Imported*/,
                            clang::SrcMgr::CharacteristicKind FileType) override;

  private:
    clang::SourceManager &SM;
    llvm::StringSet<> &Uniq;
    std::vector<std::string> &Out;
};

} // namespace Quad