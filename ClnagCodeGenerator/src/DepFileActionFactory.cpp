#include "DepFileActionFactory.h"
#include "DepFileAction.h"

Quad::DepFileActionFactory::DepFileActionFactory(std::vector<std::string> &out) : Out(out) {}

std::unique_ptr<clang::FrontendAction> Quad::DepFileActionFactory::create()
{
    return std::make_unique<DepFileAction>(Out); // ← 생성자 인자 전달 가능
}
