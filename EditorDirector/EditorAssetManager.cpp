#include "EditorAssetManager.h"
#include <Core/CoreType.h>
#include <Core/Map.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/AssetPtr.h>
#include <Logger/Logger.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <LogicalFileSystem/LogicalFolder.h>
Quad::EditorAssetManager *Quad::EditorAssetManager::GetInstance()
{
    static EditorAssetManager instance;
    return &instance;
}

Quad::EditorAssetManager::EditorAssetManager() {}

Quad::EditorAssetManager::~EditorAssetManager() {}

void Quad::EditorAssetManager::Initialize(CoreAsset::AssetManager *assetManager,
                                          QuadLF::LogicalFileSystem *logicalFileSystem,
                                          CoreAsset::AssetMetaDataManager *assetMetaDataManager)
{

    mAssetManager = assetManager;
    mLogicalFileSystem = logicalFileSystem;
    mAssetMetaDataManager = assetMetaDataManager;
}

CoreAsset::AssetPtr Quad::EditorAssetManager::CreateAsset(CoreAsset::EAssetType assetType,
                                                          CoreAsset::IntermediateAsset *intermediateAssetData,
                                                          const CoreAsset::AssetCreationContext &creationContext)
{
    QuadLF::LogicalFolder *currFolder = mLogicalFileSystem->GetCurrentLogicalFolder();

    const std::string &prefixAssetName = currFolder->GetFullPath();

    CoreAsset::AssetPtr pAsset =
        mAssetManager->CreateAsset(assetType, intermediateAssetData, prefixAssetName.c_str(), false, creationContext);

    if (pAsset.Get() == nullptr)
        return pAsset;

    CoreAsset::Asset *asset = pAsset.Get();

    // 2 결과물이 asset들에대해서 논리적파일시스템에 등록
    QuadLF::LogicalFileAssetInfo assetFileInfo;
    assetFileInfo.mAssetID = asset->GetID();
    assetFileInfo.mAssetType = asset->GetType();
    assetFileInfo.mName = asset->GetName().c_str();

    QuadLF::LogicalFile *currFile =
        mLogicalFileSystem->MakeFile(assetFileInfo, pAsset.Get()->GetName().c_str(), currFolder);

    // metaData수정
    CoreAsset::AssetMetaDataManager *assetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();
    CoreAsset::AssetMetaData *assetMetaData = assetMetaDataManager->GetMetaData(asset->GetID());
    assetMetaData->mFilePath = currFile->GetFullPath(); // 논리적 파일상대경로(물리적 파일경로이기도 하다)
    asset->SetEmptyAssetFlag(false);

    if (asset->GetAssetType() == CoreAsset::EAssetType::eMap)
    {
        mOnMapAssetAddedCallbackSystem.ExecuteCallbacks(static_cast<Map *>(asset));
    }

    return pAsset;
}

Map *Quad::EditorAssetManager::CreateNewMap()
{

    Core::IntermediateMap intermediateMap;
    intermediateMap.mAssetName = "NewMap";
    intermediateMap.mAssetType = CoreAsset::EAssetType::eMap;
    CoreAsset::AssetCreationContext context;
    context.mInitLoadState = CoreAsset::EAssetLoadState::Loaded;
    Map *newMap = CreateAsset(CoreAsset::EAssetType::eMap, &intermediateMap, context).As<Map>();

    return newMap;
}

CoreAsset::AssetPtr Quad::EditorAssetManager::DuplicateAsset(CoreAsset::AssetPtr source,
                                                             QuadLF::LogicalFolder *destFolder)
{
    if (!mAssetManager || !mLogicalFileSystem || !mAssetMetaDataManager || !source.Get())
        return nullptr;

    // 이름 결정과 파일 노드 생성은 동일한 목적지를 사용하도록 작업 시작 시 한 번만 확보한다.
    auto parentFolder = destFolder ? destFolder : mLogicalFileSystem->GetCurrentLogicalFolder();
    if (!parentFolder)
    {
        LOG_MESSAGE_ERROR("EditorAssetManager", "복제 대상 논리 폴더를 찾을 수 없습니다.");
        return nullptr;
    }

    const std::string folderPath = parentFolder->GetFullPath();

    // 원본이 엔진 에셋이어도 프로젝트 폴더에 복제하면 새 에셋은 프로젝트 domain에 속한다.
    // 폴더 이름 문자열이 아니라 실제 Engine 루트의 자손인지 확인한다.
    bool bEngine = false;
    for (QuadLF::LogicalNode *node = parentFolder; node; node = node->GetParent())
    {
        if (node == mLogicalFileSystem->GetEngineFolder())
        {
            bEngine = true;
            break;
        }
    }

    CoreAsset::AssetPtr duplicatedAsset = mAssetManager->DuplicateAsset(source, folderPath.c_str(), bEngine);
    auto asset = duplicatedAsset.Get();
    if (!asset)
        return nullptr;

    auto metaData = mAssetMetaDataManager->GetMetaData(asset->GetID());
    if (!metaData)
    {
        LOG_MESSAGE_ERROR("EditorAssetManager", "복제 에셋의 메타데이터가 없어 파일 등록에 실패했습니다.");
        return nullptr;
    }

    QuadLF::LogicalFileAssetInfo fileInfo;
    fileInfo.mAssetID = asset->GetID();
    fileInfo.mAssetType = asset->GetType();
    fileInfo.mName = asset->GetName().c_str();

    // MakeFile은 생성 알림을 동기 호출하므로, 구독자가 에셋을 조회하기 전에 저장 경로와 상태를 준비한다.
    metaData->mFilePath = folderPath + "/" + fileInfo.mName;
    asset->SetEmptyAssetFlag(false);
    auto file = mLogicalFileSystem->MakeFile(fileInfo, fileInfo.mName, parentFolder);
    if (!file)
    {
        metaData->mFilePath.clear();
        // 현재 Core에는 registry 등록 취소 API가 없다. 등록된 에셋을 직접 delete하면 dangling 참조가 남는다.
        LOG_MESSAGE_ERROR("EditorAssetManager", "복제 에셋은 registry에 남아 있으나 논리 파일 등록에 실패했습니다.");
        return nullptr;
    }
    else
    {
        const std::string message = "에셋 복제 완료: " + file->GetFullPath();
        LOG_MESSAGE_INFO("EditorAssetManager", message.c_str());
    }

    metaData->mFilePath = file->GetFullPath();

    if (asset->GetAssetType() == CoreAsset::EAssetType::eMap)
    {
        mOnMapAssetAddedCallbackSystem.ExecuteCallbacks(static_cast<Map *>(asset));
    }

    return duplicatedAsset;
}

CoreAsset::AssetPtr Quad::EditorAssetManager::GetAssetInner(const char *assetClassName, CoreAsset::AssetID id)
{

    return mAssetManager->ResolveAsset(mAssetManager->GetAssetTypeFromClassName(assetClassName), id);

    // CoreAsset::AssetPtr pAsset = mAssetManager->GetAssetFromAssetName(assetClassName, id);

    // if (pAsset.Get() != nullptr)
    //     return pAsset;

    //// 해당 id asset이없다면 디폴트로 한번더 시도

    // pAsset = mAssetManager->GetDefaultAsset(mAssetManager->GetAssetTypeFromClassName(assetClassName));

    // return pAsset;
}
