#pragma once
#include <Core/CoreDllExport.h>
#include <Core/Object.h>
#include <CoreMath/CoreMath.h>
#include <ReflectSystem/ReflectionMacro.h>

#include "Entity.generated.h"

class SceneComponent;

class CORE_API_LIB REFLECT_CLASS(EngineClass) Entity : public Object
{
    GENERATED_BODY(Entity)
  public:
    Entity();
    virtual ~Entity();

    void SetPositionLocal(const CoreMath::Vector3 &pos);
    void SetPositionLocal(float x, float y, float z);
    void SetScaleLocal(const CoreMath::Vector3 &scale);
    void SetScaleLocal(float x, float y, float z);
    void SetRotationLocal(const CoreMath::Vector3 &rotation);
    void SetRotationLocal(float x, float y, float z);
    void AddRotationLocal(float x, float y, float z);
    void AddRotationLocal(const CoreMath::Vector3 &rotation);

    void SetPositionWorld(const CoreMath::Vector3 &pos);
    void SetPositionWorld(float x, float y, float z);
    void SetScaleWorld(const CoreMath::Vector3 &scale);
    void SetScaleWorld(float x, float y, float z);
    void SetRotationWorld(const CoreMath::Vector3 &rotation);
    void SetRotationWorld(float x, float y, float z);

    void TranslationWorld(float x, float y, float z);
    void TranslationWorld(const CoreMath::Vector3 &shift);

    CoreMath::Vector3 GetPositionLocal() const;
    CoreMath::Vector3 GetScaleLocal() const;
    CoreMath::Vector3 GetRotationLocal() const;

    CoreMath::Vector3 GetPositionWorld() const;
    CoreMath::Vector3 GetScaleWorld() const;
    CoreMath::Vector3 GetRotationWorld() const;

    CoreMath::Vector3 GetForwardWorld() const;
    CoreMath::Vector3 GetRightWorld() const;
    CoreMath::Vector3 GetUpWorld() const;

    void SetRootComponent(SceneComponent *com);
    SceneComponent *GetRootComponent() const;
    virtual void OnConnectedNewParent(Object *parent) override;
    virtual void OnDisconnectedParent() override;

    virtual void Serialize(Arch &arch) override;

  protected:
    SceneComponent *mRootSceneComponent;


    virtual void OnBegin() override;

  private:
    void CleanUpOrphanSceneComponents();

  private:
    SceneComponent *mDefaultRootSceneComponent;
};
