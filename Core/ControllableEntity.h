#pragma once

#include <Core/Entity.h>

#include "ControllableEntity.generated.h"

class CORE_API_LIB REFLECT_CLASS(EngineClass) ControllableEntity : public Entity
{

    GENERATED_BODY(ControllableEntity)
  public:
    ControllableEntity();
    virtual ~ControllableEntity();

  protected:
    virtual void OnBegin() override;

  private:
};
