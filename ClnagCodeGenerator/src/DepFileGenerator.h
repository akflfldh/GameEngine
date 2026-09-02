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
class DepFileGenerator
{
  public:
    DepFileGenerator();
    ~DepFileGenerator();

    void Initialize(clang::tooling::CommonOptionsParser &OptionsParser);

    bool Run();

  private:
    bool GeneratorDepFile(const std::vector<std::string> &depFileVector);

  private:
    std::unique_ptr<clang::tooling::ClangTool> mClangTool;
};

} // namespace Quad