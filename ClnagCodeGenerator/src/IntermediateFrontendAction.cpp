
#include "IntermediateFrontendAction.h"
#include "DepFileCollector.h"
#include "IntermediateASTConsumer.h"

Quad::IntermediateFrontendAction::IntermediateFrontendAction(std::vector<std::string> &out) : Out(out) {}

bool Quad::IntermediateFrontendAction::BeginSourceFileAction(clang::CompilerInstance &CI)
{
    CI.getFrontendOpts().SkipFunctionBodies = true; // ★ 함수 본문 파싱 건너뛰기
    // 옵션: 오타 교정/컬러 비활성화로 약간 더 줄이기
    CI.getDiagnosticOpts().ShowColors = false;
    CI.getDiagnosticOpts().ShowNoteIncludeStack = false;

    return CollectIncludeFIles(CI);
}

std::unique_ptr<clang::ASTConsumer> Quad::IntermediateFrontendAction::CreateASTConsumer(clang::CompilerInstance &CI,
                                                                                        llvm::StringRef InFile)
{

    llvm::SmallString<260> p(InFile);
    std::string Base = llvm::sys::path::filename(p).str();
    // 확장자 제거
    llvm::StringRef BaseRef(Base);
    if (BaseRef.ends_with_insensitive(".h") || BaseRef.ends_with_insensitive(".hpp") ||
        BaseRef.ends_with_insensitive(".hh") || BaseRef.ends_with_insensitive(".cpp") ||
        BaseRef.ends_with_insensitive(".cc") || BaseRef.ends_with_insensitive(".cxx"))
    {
        Base = llvm::sys::path::stem(BaseRef).str();
    }

    return std::make_unique<IntermediateASTConsumer>();
}

bool Quad::IntermediateFrontendAction::CollectIncludeFIles(clang::CompilerInstance &CI)
{

    // 전처리기가 #include지시문을 처리할떄 파일목록들을 추적하게 하자 .
    // 그렇게해서 deps파일을 만들자.
    // auto &CI = getCompilerInstance();
    auto &PP = CI.getPreprocessor();
    clang::SourceManager &SM = CI.getSourceManager();

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
    return true;
}
