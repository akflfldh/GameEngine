#include "StaticMeshObject.h"
#include <Core/StaticMeshComponent.h>
Core::StaticMeshObject::StaticMeshObject()
{
    mStaticMeshComponent =
        static_cast<StaticMeshComponent *>(CreateDefaultComponent<StaticMeshComponent>("StaticMeshCom"));
    mStaticMeshComponent->SetParent(mRootSceneComponent);
}

Core::StaticMeshObject::~StaticMeshObject() {}

void Core::StaticMeshObject::OnBegin()
{

    Entity::OnBegin();
}

void Core::StaticMeshObject::Serialize(Arch &arch)
{

    Entity::Serialize(arch);

    mStaticMeshComponent->Serialize(arch);
}
