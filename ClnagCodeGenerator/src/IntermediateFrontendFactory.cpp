#include "IntermediateFrontendFactory.h"

#include "IntermediateFrontendAction.h"

Quad::IntermediateFrontendFactory::IntermediateFrontendFactory(std::vector<std::string> &out) : Out(out) {}

std::unique_ptr<clang::FrontendAction> Quad::IntermediateFrontendFactory::create()
{
    return std::make_unique<IntermediateFrontendAction>(Out);
}
