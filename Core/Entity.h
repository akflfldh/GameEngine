#pragma once
#include <Core/CoreDllExport.h>
#include <Core/Object.h>
#include <ReflectSystem/ReflectionMacro.h>

#include <Core/Generated/Entity.generated.h>

namespace Core
{
class SceneComponent;

class CORE_API_LIB REFLECT_CLASS Entity : public Object
{
    GENERATED_BODY(Entity)
  public:
    Entity();
    virtual ~Entity();

    virtual void Start() override;
    virtual void Update();

  private:
    SceneComponent *mRootSceneComponent;
};

} // namespace Core