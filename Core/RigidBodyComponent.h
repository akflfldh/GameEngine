//#pragma once
//
//#include <Core/Component.h>
//#include <Core/CoreDllExport.h>
//#include <Core/IPhysicsBodyComponent.h>
//
//#include "RigidBodyComponent.generated.h"
//
//class CORE_API_LIB REFLECT_CLASS(EngineClass) RigidBodyComponent : public Component, public IPhysicsBodyComponent
//{
//  public:
//    RigidBodyComponent();
//    virtual ~RigidBodyComponent();
//
//    void SetPhysicsEnabled(bool flag);
//    void SetBodyType(EPhysicsBodyType type);
//    void SetUseGravity(bool flag);
//    void SetMass(float mass);
//
//    virtual bool IsPhysicsEnabled() const override;
//    virtual EPhysicsBodyType GetPhysicsBodyType() const override;
//    virtual bool IsPhysicsGravityEnabled() const override;
//    virtual float GetPhysicsMass() const override;
//
//    virtual void OnOwnerObjectAddedToMap() override;
//
//  private:
//    bool mPhysicsEnabled = false;
//    EPhysicsBodyType mPhysicsBodyType = EPhysicsBodyType::eStatic;
//    bool mUseGravity = true;
//    float mMass = 1.0f;
//};
