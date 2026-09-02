#pragma once

#include <Core/Entity.h>

#include "StaticMeshObject.generated.h"

class StaticMeshComponent;

namespace Core
{
class CORE_API_LIB REFLECT_CLASS(EngineClass) StaticMeshObject : public Entity
{
    GENERATED_BODY(StaticMeshObject)
  public:
    StaticMeshObject();
    virtual ~StaticMeshObject();

    virtual void OnBegin();
    virtual void Serialize(Arch &arch) override;

    REFLECT_PROPERTY()
    StaticMeshComponent *mStaticMeshComponent;

  private:
};
} // namespace Core
