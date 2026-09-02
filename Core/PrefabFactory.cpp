#include "PrefabFactory.h"

#include <Core/CoreType.h>
#include <Core/Object.h>
#include <Core/Prefab.h>
#include <ReflectSystem/ReflectionSystem.h>

PrefabFactory *PrefabFactory::GetInstance()
{
    static PrefabFactory instance;
    return &instance;
}

PrefabFactory::PrefabFactory() {}

PrefabFactory::~PrefabFactory() {}

CoreAsset::Asset *PrefabFactory::CreateEmptyAsset(CoreAsset::EAssetType assetType)
{
    return new Prefab;
}

CoreAsset::Asset *PrefabFactory::CreateAssetFromData(const CoreAsset::IntermediateAsset &serializedAsset)
{

    const Core::IntermediatePrefab &intermediatePrefab = static_cast<const Core::IntermediatePrefab &>(serializedAsset);

    Prefab *prefab = static_cast<Prefab *>(CreateEmptyAsset(CoreAsset::EAssetType::ePrefab));

    auto reflectionSystem = Quad::ReflectionSystem::GetInstance();
    Object *object =
        static_cast<Object *>(reflectionSystem->CreateClassInstance(intermediatePrefab.mClassName.c_str()));

    if (object)
    {
        prefab->SetName(intermediatePrefab.mAssetName);
        prefab->mDefaultObject = object;
    }

    return prefab;
}
