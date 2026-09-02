#include "IntermediateGenerator.h"
#include "IntermediateFrontendAction.h"
#include "IntermediateFrontendFactory.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"

extern llvm::cl::opt<std::string> OptDepfile;
// 새로 추가할 depfile 타겟 옵션
extern llvm::cl::opt<std::string> OptDepTarget;

Quad::InterrmediateGenerator::InterrmediateGenerator() {}

Quad::InterrmediateGenerator::~InterrmediateGenerator() {}

void Quad::InterrmediateGenerator::Initialize(clang::tooling::CommonOptionsParser &OptionsParser)
{
    mClangTool =
        std::make_unique<clang::tooling::ClangTool>(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
}

int Quad::InterrmediateGenerator::Run()
{

    std::vector<std::string> depFileVector;

    llvm::errs() << "[DEBUG] Main: ClangTool is about to run...\n";
    int ret = mClangTool->run(std::make_unique<IntermediateFrontendFactory>(depFileVector).get());
    llvm::errs() << "[DEBUG] Main: ClangTool finished successfully. Deps count: " << depFileVector.size() << "\n";

    if (ret != 0)
        return ret;

    return GeneratorDepFile(depFileVector);
}

int Quad::InterrmediateGenerator::GeneratorDepFile(const std::vector<std::string> &depFileVector)
{

    if (!OptDepfile.empty() && !OptDepTarget.empty())
    {
        // 1) 타겟(stamp) 경로: 절대 + / 로 통일
        llvm::SmallString<260> tgt(OptDepTarget.getValue());
        llvm::sys::fs::make_absolute(tgt);
        llvm::sys::path::remove_dots(tgt, true);
        std::string tgtSlash = llvm::sys::path::convert_to_slash(tgt.str());

        // 2) depfile 출력 디렉터리 생성
        llvm::SmallString<260> out(OptDepfile.getValue());
        auto parent = llvm::sys::path::parent_path(out);
        if (!parent.empty())
            llvm::sys::fs::create_directories(parent);

        // 3) deps에서 .generated 파일들 제거
        std::vector<std::string> filteredDeps;
        for (const auto &dep : depFileVector)
        {
            if (dep.find(".generated.") == std::string::npos)
            {
                filteredDeps.push_back(dep);
            }
            else
            {
                llvm::errs() << "[FILTER] Removing .generated dependency: " << dep << "\n";
            }
        }

        // 4) depfile 쓰기: "<target>: <dep1> <dep2> ..."
        std::error_code ec;
        llvm::raw_fd_ostream os(out, ec, llvm::sys::fs::OF_Text);
        if (ec)
        {
            llvm::errs() << "write depfile failed: " << ec.message() << "\n";
            return -1;
        }

        os << tgtSlash << ":"; // 타겟
        for (auto &p : filteredDeps)
            os << " " << p; // 필터링된 deps
        os << "\n";

        // 디버그: 최종 deps 출력
        llvm::errs() << "[DEBUG] Final deps written to " << out << ":\n";
        llvm::errs() << "Target: " << tgtSlash << "\n";
        for (const auto &dep : filteredDeps)
        {
            llvm::errs() << "  Dep: " << dep << "\n";
        }

        return 0;
    }

    // depfile도 outdeps도 없으면 할 일이 없음
    llvm::errs() << "deps-only mode requires --depfile and --deptarget\n";
    return -1;
}
