#pragma once

#include <Core/Entity.h>

#include "PlayerStart.generated.h"

class CORE_API_LIB REFLECT_CLASS(EngineClass) PlayerStart : public Entity
{
    GENERATED_BODY(PlayerStart)
  public:
    PlayerStart();
    virtual ~PlayerStart();

  protected:
    virtual void OnAddedToMap() override;

  private:
};
