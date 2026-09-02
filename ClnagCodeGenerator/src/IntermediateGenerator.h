#pragma once
#include <memory>
#include <string>
#include <vector>
namespace clang
{
namespace tooling
{
class ClangTool;
class CommonOptionsParser;
} // namespace tooling
} // namespace clang

namespace Quad
{

class InterrmediateGenerator
{

  public:
    InterrmediateGenerator();
    ~InterrmediateGenerator();

    void Initialize(clang::tooling::CommonOptionsParser &OptionsParser);
    int Run();

  private:
    int GeneratorDepFile(const std::vector<std::string> &depFileVector);

  private:
    std::unique_ptr<clang::tooling::ClangTool> mClangTool;
};
} // namespace Quad