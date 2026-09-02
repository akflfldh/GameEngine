#include "DepFileAction.h"
#include "DepFileCollector.h"
#include "clang/Frontend/CompilerInstance.h"
#include <memory>
Quad::DepFileAction::DepFileAction(std::vector<std::string> &out) : Out(out) {}

void Quad::DepFileAction::ExecuteAction()
{

    auto &CI = getCompilerInstance();
    auto &PP = CI.getPreprocessor();
    clang::SourceManager &SM = CI.getSourceManager();

    llvm::StringSet<> Uniq;

    // ★ 메인 파일을 먼저 Out에 추가 (절대경로 + /)
    if (const clang::FileEntry *MainFE = SM.getFileEntryForID(SM.getMainFileID()))
    {
        llvm::StringRef rp = MainFE->tryGetRealPathName();
        llvm::StringRef name = rp.empty() ? MainFE->getName() : rp;

        // ★ 메인 파일이 .generated.가 아닌 경우만 추가
        if (!name.contains(".generated."))
        {
            llvm::SmallString<260> abs(name);
            llvm::sys::fs::make_absolute(abs);
            llvm::sys::path::remove_dots(abs, true);
            std::string absSlash = llvm::sys::path::convert_to_slash(abs.str());

            if (Uniq.insert(absSlash).second)
            {
                llvm::errs() << "[MAIN] Adding main file: " << absSlash << "\n";
                Out.emplace_back(std::move(absSlash));
            }
        }
        else
        {
            llvm::errs() << "[SKIP] Main file is .generated, skipping: " << name << "\n";
        }
    }

    // include 콜백 등록해서 사용자 헤더들을 계속 수집
    PP.addPPCallbacks(std::make_unique<DepFileCollector>(SM, Uniq, Out));

    // 전처리만 수행
    clang::PreprocessOnlyAction::ExecuteAction();

    // 디버그: 최종 deps 목록 출력
    llvm::errs() << "[FINAL] Total dependencies found: " << Out.size() << "\n";
    for (const auto &dep : Out)
    {
        llvm::errs() << "  - " << dep << "\n";
    }
}