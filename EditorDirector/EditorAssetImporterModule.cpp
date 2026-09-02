#include "EditorAssetImporterModule.h"
#include <CoreAsset/AssetImporterManager.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/AssetMetaDataType.h>
#include <CoreBase/AsyncThreadPool.h>
#include <CoreBase/FVector.h>
#include <EditorDirector/EditorFontImporter.h>
#include <ImportModule/FBXImporter.h>
#include <ImportModule/TextureImporter.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <RenderFrontend/AssetResolver.h>
Quad::EditorAssetImporterModule *Quad::EditorAssetImporterModule::GetInstance()
{

    static EditorAssetImporterModule instance;
    return &instance;
}

Quad::EditorAssetImporterModule::EditorAssetImporterModule() {}

Quad::EditorAssetImporterModule::~EditorAssetImporterModule() {}

void Quad::EditorAssetImporterModule::Initialize()
{
    CoreAsset::AssetImporterManager *assetImporterManager = CoreAsset::AssetImporterManager::GetInstance();

    Import::TextureImporter *textureImporter = Import::TextureImporter::GetInstance();

    assetImporterManager->RegisterAssetImporter(".png", textureImporter);
    assetImporterManager->RegisterAssetImporter(".jpg", textureImporter);
    assetImporterManager->RegisterAssetImporter(".bmp", textureImporter);
    assetImporterManager->RegisterAssetImporter(".dds", textureImporter);

    EditorFontImporter *fontImporter = EditorFontImporter::GetInstance();

    assetImporterManager->RegisterAssetImporter("fda", fontImporter);

    Import::FBXImporter *fbxImporter = Import::FBXImporter::GetInstance();
    fbxImporter->Initialize();
    assetImporterManager->RegisterAssetImporter(".fbx", fbxImporter);
}

ImportResult Quad::EditorAssetImporterModule::Import(const std::filesystem::path &file, bool bEngine,
                                                     ImportJobContext *jobContext)
{

    ImportResult importResult;

    // 1. assetManager에게 import요청
    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();

    QuadLF::LogicalFileSystem *logicalFileSystem = QuadLF ::LogicalFileSystem::GetInstance();

    // auto preLogicalFolder = logicalFileSystem->GetCurrentLogicalFolder();
    // if (bEngine)
    //{
    //     auto engineFolder = logicalFileSystem->GetFolder("/Engine");
    //     logicalFileSystem->SetCurrentLogicalFolder(engineFolder);
    // }

    // 현재 논리적파일시스템의 현재 폴더경로도 같이 넘긴다.
    const std::string currLogicalFolderPath = logicalFileSystem->GetCurrentLogicalFolderPath();

    std::vector<CoreAsset::Asset *> importedAssets =
        assetManager->ImportAsset(file, currLogicalFolderPath.c_str(), bEngine);

    if (importedAssets.size() == 0)
    {
        if (jobContext)
            jobContext->ReportFailed("임포트된 에셋이 없습니다.");
        return importResult;
    }

    if (jobContext)
        jobContext->ReportProgress(0.6f, "에셋 gpu 업로드 중");

    Render::AssetResolver *assetResovler = Render::AssetResolver::GetInstance();
    for (auto asset : importedAssets)
    {
        if (asset->GetType() == CoreAsset::EAssetType::eStaticMesh)
        {
            // mesh import 단계에서 gpu upload까지 수행해야한다.
            assetResovler->ResolveAsset(asset);
        }
    }

    //// 이거는 메인스레드가 하도록 옮기자
    ////  2 결과물이 asset들에대해서 논리적파일시스템에 등록
    // CoreAsset::AssetMetaDataManager *assetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();

    // for (size_t i = 0; i < importedAssets.size(); ++i)
    //{
    //     CoreAsset::Asset *asset = importedAssets[i];
    //     if (asset == nullptr)
    //         continue;

    //    QuadLF::LogicalFileAssetInfo texturelogicalFileInfo;
    //    texturelogicalFileInfo.mAssetID = asset->GetID();
    //    texturelogicalFileInfo.mAssetType = asset->GetType();
    //    texturelogicalFileInfo.mName = asset->GetName().c_str();

    //    QuadLF::LogicalFile *LogicalFile = logicalFileSystem->MakeFile(
    //        texturelogicalFileInfo, asset->GetName().c_str(), logicalFileSystem->GetCurrentLogicalFolder(), false);

    //    CoreAsset::AssetMetaData *assetMetaData = assetMetaDataManager->GetMetaData(asset->GetID());

    //    assetMetaData->mFilePath = LogicalFile->GetFullPath(); // 논리적 파일상대경로(물리적 파일경로이기도 하다)
    //}

    // logicalFileSystem->SetCurrentLogicalFolder(preLogicalFolder);

    if (jobContext)
        jobContext->ReportCompeleted("임포트 완료", importedAssets);

    importResult.mSuccess = true;
    importResult.mAssets = std::move(importedAssets);

    return importResult;
}

ImportTaskHandle Quad::EditorAssetImporterModule::RequestImport(const std::string &file, bool bEngine)
{

    AsyncThreadPool *threadPool = AsyncThreadPool::GetInstance();

    std::shared_ptr<ImportNotifyChannel> notifyChannel = std::make_shared<ImportNotifyChannel>();
    ImportTaskHandle importTaskHandle;
    importTaskHandle.mNotifyChannel = notifyChannel;

    threadPool->Submit(
        [this, file, bEngine, notifyChannel]()
        {
            ImportJobContext context(notifyChannel);
            Import(file.c_str(), bEngine, &context);
        });

    return importTaskHandle;
}

ImportResult Quad::EditorAssetImporterModule::RequestImportSync(const std::filesystem::path &file, bool bEngine)
{
    return Import(file, bEngine, nullptr);
}

ImportResult Quad::EditorAssetImporterModule::ImportDendencySync(const char *file, bool bEngine)
{

    return Import(file, bEngine, nullptr);
}
