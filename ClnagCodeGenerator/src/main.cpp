#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Clang 라이브러리에서 필요한 헤더 파일들을 가져옵니다.
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

#include "IntermediateGenerator.h"
#include <iostream>
static llvm::cl::OptionCategory MyToolCategory("My Code Generator Options");

static llvm::cl::SubCommand ScanMode("scan", "Generate Intermediate Json File");
static llvm::cl::SubCommand EmitMode("emit", "Generate genFile From Intermediate Json File");

/* ▼▼ 추가 ▼▼  */
llvm::cl::opt<std::string> OutputDir( // ← 전역 옵션 선언
    "outdir", llvm::cl::desc("출력 경로 설정"), llvm::cl::value_desc("path"),
    llvm::cl::init("Generated"), // 기본값
    llvm::cl::cat(MyToolCategory), llvm::cl::sub(ScanMode));
/* ▲▲ 추가 ▲▲  */

llvm::cl::opt<std::string> OutputJson( // ← 전역 옵션 선언
    "outputJson", llvm::cl::desc("출력 json파일"), llvm::cl::value_desc("path"),
    llvm::cl::init(""), // 기본값
    llvm::cl::cat(MyToolCategory), llvm::cl::sub(ScanMode));

// 새로 추가할 depfile 경로 옵션
llvm::cl::opt<std::string> OptDepfile("depfile", llvm::cl::desc("Path to write Make-style depfile"),
                                      llvm::cl::value_desc("path"), llvm::cl::cat(MyToolCategory),
                                      llvm::cl::sub(ScanMode));

llvm::cl::opt<std::string> OutputGPP("outputGpp", llvm::cl::desc("출력 gpp파일"), llvm::cl::value_desc("path"),
                                     llvm::cl::init(""), llvm::cl::cat(MyToolCategory), llvm::cl::sub(ScanMode));

llvm::cl::opt<std::string> OutputGH("outputGh", llvm::cl::desc("출력 gh파일"), llvm::cl::value_desc("path"),
                                    llvm::cl::init(""), llvm::cl::cat(MyToolCategory), llvm::cl::sub(ScanMode));

// 새로 추가할 depfile 타겟 옵션
llvm::cl::opt<std::string> OptDepTarget("deptarget",
                                        llvm::cl::desc("Target path to use in depfile (must match CMake OUTPUT)"),
                                        llvm::cl::value_desc("path"), llvm::cl::cat(MyToolCategory),
                                        llvm::cl::sub(ScanMode));

// 4. 메인 함수 (프로그램의 시작점)
int main(int argc, const char **argv)
{
    std::cout << "start\n";
    auto ExpectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, MyToolCategory);
    if (!ExpectedParser)
    {
        llvm::errs() << ExpectedParser.takeError();
        llvm::outs() << "실패\n";
        return 1;
    }
    clang::tooling::CommonOptionsParser &OptionsParser = ExpectedParser.get();

    std::cout << "parser ok \n";

    auto it = llvm::cl::getRegisteredSubcommands();
    if (ScanMode)
    {
        std::cout << "scan mode start \n";
        Quad::InterrmediateGenerator intermediateGen;
        intermediateGen.Initialize(OptionsParser);
        return intermediateGen.Run();
    }
    else if (EmitMode)
    {
        std::cout << "emit mode start \n";
    }
    std::cout << "end \n";
}