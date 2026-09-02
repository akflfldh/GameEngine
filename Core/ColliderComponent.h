#pragma once

#include <Core/Component.h>

#include "ColliderComponent.generated.h"

class CORE_API_LIB REFLECT_CLASS(EngineClass) ColliderComponent : public Component
{
    GENERATED_BODY(ColliderComponent)
  public:
    ColliderComponent();
    virtual ~ColliderComponent();



  private:
};
