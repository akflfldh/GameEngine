#include "LightObject.h"

#include <Core/LightComponent.h>

Core::LightObject::LightObject()
{
    mLightComponent = static_cast<LightComponent *>(CreateDefaultComponent<LightComponent>("LightCom"));
    mLightComponent->SetStrength({1, 1, 1});

    mLightComponent->SetParent(mRootSceneComponent);
}

Core::LightObject::~LightObject() {}

void Core::LightObject::OnBegin()
{

    Entity::OnBegin();
}

void Core::LightObject ::Update(float DeltaTime)
{

    Entity::Update(DeltaTime);
}

void Core::LightObject::SetLightType(ELightType type)
{

    mLightComponent->SetLightType(type);
}
Core::ELightType Core::LightObject::GetLightType() const
{
    return mLightComponent->GetLightType();
}

CoreMath::Vector3 Core::LightObject::GetStrength() const
{
    return mLightComponent->GetStrength();
}

float Core::LightObject::GetFalloffStart() const
{
    return mLightComponent->GetFalloffStart();
}

float Core::LightObject::GetFalloffEnd() const
{
    return mLightComponent->GetFalloffEnd();
}

float Core::LightObject::GetSpotPower() const
{
    return mLightComponent->GetSpotPower();
}

void Core::LightObject::SetStrength(const CoreMath::Vector3 &strength)
{
    if (mLightComponent)
    {
        mLightComponent->SetStrength(strength);
    }
}

void Core::LightObject::SetFalloffStart(float value)
{
    if (mLightComponent)
    {
        mLightComponent->SetFalloffStart(value);
    }
}

void Core::LightObject::SetFalloffEnd(float value)
{

    if (mLightComponent)
    {
        mLightComponent->SetFalloffEnd(value);
    }
}

void Core::LightObject::SetSpotPower(float value)
{

    if (mLightComponent)
    {
        mLightComponent->SetSpotPower(value);
    }
}

void Core::LightObject::EndTick(float deltaTime)
{
    Entity::EndTick(deltaTime);
}

void Core::LightObject::Serialize(Arch &arch)
{

    Entity::Serialize(arch);

    mLightComponent->Serialize(arch);
}
