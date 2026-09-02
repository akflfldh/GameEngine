#pragma once

#include "clang/Frontend/FrontendActions.h"
#include <string>
#include <vector>
namespace Quad
{

class DepFileAction : public clang::PreprocessOnlyAction
{
  public:
    DepFileAction(std::vector<std::string> &out);

  protected:
    void ExecuteAction() override;

  private:
    std::vector<std::string> &Out;
};

} // namespace Quad