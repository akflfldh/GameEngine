#pragma once

#include "clang/Tooling/Tooling.h"
#include <string>
#include <vector>

namespace Quad
{

class DepFileActionFactory : public clang::tooling::FrontendActionFactory
{
  public:
    explicit DepFileActionFactory(std::vector<std::string> &out);
    std::unique_ptr<clang::FrontendAction> create() override;

  private:
    std::vector<std::string> &Out; // 외부 벡터를 참조로 보관
};
} // namespace Quad