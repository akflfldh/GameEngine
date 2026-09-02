#include "PrefabGenerationManager.h"
#include <Core/CoreType.h>
#include <Core/Prefab.h>
#include <CoreAsset/AssetManager.h>
#include <CoreBase/CoreAssert.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <LogicalFileSystem/LogicalFolder.h>

#include <EditorDirector/EditorAssetManager.h>
PrefabGenerationManager *PrefabGenerationManager::GetInstance()
{
    PrefabGenerationManager instance;

    return &instance;
}

PrefabGenerationManager::PrefabGenerationManager() {}

PrefabGenerationManager::~PrefabGenerationManager() {}

Prefab *PrefabGenerationManager::CreatePrefab(const std::string &parentClassName, const std::string &prefabName)
{
    auto editorAssetManager = Quad::EditorAssetManager::GetInstance();

    Core::IntermediatePrefab IntermediatePrefab;
    IntermediatePrefab.mAssetName = prefabName.c_str();
    IntermediatePrefab.mAssetType = CoreAsset::EAssetType::ePrefab;
    IntermediatePrefab.mClassName = parentClassName;

    auto logicalFileSystem = QuadLF::LogicalFileSystem::GetInstance();
    QuadLF::LogicalFolder *preFolder = logicalFileSystem->GetCurrentLogicalFolder();
    logicalFileSystem->SetCurrentLogicalFolder(logicalFileSystem->GetAssetFolder());

    CoreAsset::Asset *asset =
        editorAssetManager->CreateAsset(CoreAsset::EAssetType::ePrefab, &IntermediatePrefab).Get();
    logicalFileSystem->SetCurrentLogicalFolder(preFolder);

    Prefab *prefab = static_cast<Prefab *>(asset);
    prefab->EnsureDefaultComponentPrefabKey();

    // auto assetManager = CoreAsset::AssetManager::GetInstance();

    // const std::string &path = logicalFileSystem->GetAssetFolder()->GetFullPath();

    //// 프리팹 생성
    // CoreAsset::Asset *asset =
    //     assetManager->CreateAsset(CoreAsset::EAssetType::ePrefab, IntermediatePrefab, path.c_str()).Get();

    //// 파일시스템에 등록
    // QuadLF::LogicalFileAssetInfo fileAssetInfo;
    // CHECK(logicalFileSystem->MakeFile(fileAssetInfo, asset->GetName().c_str(), logicalFileSystem->GetAssetFolder())
    // !=
    //       nullptr);

    return static_cast<Prefab *>(asset);
}
