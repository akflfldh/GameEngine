#pragma once

#include "clang/Tooling/Tooling.h"
#include <memory>
#include <string>
#include <vector>
namespace Quad
{

class IntermediateFrontendFactory : public clang::tooling::FrontendActionFactory
{
  public:
    explicit IntermediateFrontendFactory(std::vector<std::string> &out);
    std::unique_ptr<clang::FrontendAction> create() override;

  private:
    std::vector<std::string> &Out; // 외부 벡터를 참조로 보관
};
} // namespace Quad