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

ImportResult Quad::EditorAssetImporterModule::Import(const EditorImportRequest &importRequest,
                                                     ImportJobContext *jobContext)
{

    ImportResult importResult;

    // 1. assetManager에게 import요청
    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();
    QuadLF::LogicalFileSystem *logicalFileSystem = QuadLF ::LogicalFileSystem::GetInstance();

    // 현재 논리적파일시스템의 현재 폴더경로도 같이 넘긴다.
    const std::string currLogicalFolderPath = logicalFileSystem->GetCurrentLogicalFolderPath();

    CoreAsset::AssetImportContext importContext;
    importContext.mCreationContextTable;
    importContext.mEngineAsset = importRequest.bEngine;
    importContext.mRegistryPrefix = currLogicalFolderPath;

    CoreAsset::ImportExecutionContext importExecutionContext = Convert(importRequest);

    std::vector<CoreAsset::Asset *> importedAssets =
        assetManager->ImportAsset(importRequest.mSourcePath, importContext, importExecutionContext);

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

    if (jobContext)
        jobContext->ReportCompeleted("임포트 완료", importedAssets);

    importResult.mSuccess = true;
    importResult.mAssets = std::move(importedAssets);

    return importResult;
}

ImportResult Quad::EditorAssetImporterModule::Import(const std::filesystem::path &file,
                                                     const CoreAsset::AssetImportContext &assetImportContext)
{
    ImportResult importResult;

    // 1. assetManager에게 import요청
    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();

    QuadLF::LogicalFileSystem *logicalFileSystem = QuadLF ::LogicalFileSystem::GetInstance();

    // 현재 논리적파일시스템의 현재 폴더경로도 같이 넘긴다.
    const std::string currLogicalFolderPath = logicalFileSystem->GetCurrentLogicalFolderPath();

    CoreAsset::ImportExecutionContext importExecutionContext;

    std::vector<CoreAsset::Asset *> importedAssets =
        assetManager->ImportAsset(file, assetImportContext, importExecutionContext);

    Render::AssetResolver *assetResovler = Render::AssetResolver::GetInstance();
    for (auto asset : importedAssets)
    {
        if (asset->GetType() == CoreAsset::EAssetType::eStaticMesh)
        {
            // mesh import 단계에서 gpu upload까지 수행해야한다.
            assetResovler->ResolveAsset(asset);
        }
    }
    importResult.mSuccess = true;
    importResult.mAssets = std::move(importedAssets);

    return importResult;
}

CoreAsset::ImportExecutionContext Quad::EditorAssetImporterModule::Convert(
    const EditorImportRequest &importRequest) const
{

    CoreAsset::ImportExecutionContext executionContext;

    switch (importRequest.mAssetImportType)
    {
    case EAssetImporterType::eTexture:

        executionContext.mTextureContext.bSRGB = importRequest.mTextureSettings.bSRGB;
        break;

    case EAssetImporterType::eFbx:

        break;

    case EAssetImporterType::eFont:

        break;
    }

    return executionContext;
}

ImportTaskHandle Quad::EditorAssetImporterModule::RequestImport(const std::filesystem::path &file, bool bEngine)
{

    AsyncThreadPool *threadPool = AsyncThreadPool::GetInstance();

    std::shared_ptr<ImportNotifyChannel> notifyChannel = std::make_shared<ImportNotifyChannel>();
    ImportTaskHandle importTaskHandle;
    importTaskHandle.mNotifyChannel = notifyChannel;

    threadPool->Submit(
        [this, file, bEngine, notifyChannel]()
        {
            ImportJobContext context(notifyChannel);
            EditorImportRequest importRequest;

            Import(importRequest, &context);
        });

    return importTaskHandle;
}

ImportTaskHandle Quad::EditorAssetImporterModule::RequestImport(const EditorImportRequest &importRequest)
{

    AsyncThreadPool *threadPool = AsyncThreadPool::GetInstance();

    std::shared_ptr<ImportNotifyChannel> notifyChannel = std::make_shared<ImportNotifyChannel>();
    ImportTaskHandle importTaskHandle;
    importTaskHandle.mNotifyChannel = notifyChannel;

    threadPool->Submit(
        [this, importRequest, notifyChannel]()
        {
            ImportJobContext context(notifyChannel);
            Import(importRequest, &context);
        });

    return importTaskHandle;
}

ImportResult Quad::EditorAssetImporterModule::RequestImportSync(const std::filesystem::path &file, bool bEngine)
{
    EditorImportRequest importRequest;
    importRequest.bEngine = bEngine;
    importRequest.mSourcePath = file;
    return Import(importRequest, nullptr);
}

ImportResult Quad::EditorAssetImporterModule::RequestImportSync(const std::filesystem::path &file,
                                                                const CoreAsset::AssetImportContext &assetImportContext)
{
    return Import(file, assetImportContext);
}

ImportResult Quad::EditorAssetImporterModule::ImportDendencySync(const char *file, bool bEngine)
{
    EditorImportRequest importRequest;
    importRequest.bEngine = bEngine;
    importRequest.mSourcePath = file;
    return Import(importRequest, nullptr);
}
