#include "Character.h"
#include <Core/AnimatorComponent.h>
#include <Core/CharacterMovementComponent.h>
#include <Core/StaticMeshComponent.h>
#include <CoreAsset/StaticMesh.h>

Character::Character()
{
    mMovementComponent = static_cast<CharacterMovementComponent *>(
        CreateDefaultComponent<CharacterMovementComponent>("MovementComponent"));

    // mStaticMeshComponent =
    //     static_cast<StaticMeshComponent *>(CreateDefaultComponent<StaticMeshComponent>("StaticMeshCom"));
    // mStaticMeshComponent->SetParent(mRootSceneComponent);
    //// mStaticMeshComponent->SetComponentFlag(Core::EComponentFlag::eEngineAdded);
    // mStaticMeshComponent->SetPhysicsBodyType(EPhysicsBodyType::eKinematic);

    //    mMovementComponent->SetUpdatedPhysicsComponent(mStaticMeshComponent);
}

Character ::~Character() {}

void Character::OnBegin()
{

    ControllableEntity::OnBegin();

    if (mMovementComponent != nullptr && mStaticMeshComponent != nullptr)
    {
        mMovementComponent->SetUpdatedPhysicsComponent(mStaticMeshComponent);
    }
}

bool Character::PlayClip(CoreAsset::AssetPtr clip)
{

    AnimatorComponent *animatorCom = GetComponent<AnimatorComponent>();

    if (animatorCom == nullptr)
        return false;

    return animatorCom->PlayClip(clip);
}

bool Character::PlayClip(const std::string &clipAssetName)
{

    AnimatorComponent *animatorCom = GetComponent<AnimatorComponent>();

    if (animatorCom == nullptr)
        return false;

    return animatorCom->PlayClip(clipAssetName);
}
