#pragma once

#include <glm/glm.hpp>
namespace UI
{
class IViewContextInterface
{
  public:
    virtual glm::vec2 ConverToWorldPos(int clientPosX, int clientPosY) const = 0;

  private:
};

} // namespace UI