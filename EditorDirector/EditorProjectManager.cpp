#include "EditorDirector/EditorProjectManager.h"

#include "EditorDirector/EditorShaderImporter.h"
#include "EditorDirector/EditorTextureImporter.h"
#include "EditorDirector/SerializedAssetContainer.h"
#include <BinaryReaderWriter/BinaryReader.h>
#include <Core/ProjectConfig.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/GpuSamplerSystem.h>

#include <CoreAsset/AssetCommon.h>
#include <CoreAsset/AssetLoader.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/GlobalAssetRegistrySystem.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/MaterialManager.h>
#include <CoreAsset/MaterialStorer.h>
#include <CoreAsset/Texture.h>
#include <CoreAsset/TextureManager.h>
#include <CoreAsset/UIMaterialManager.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <EditorDirector/EditorConfig.h>
#include <ImportModule/TextureImporter.h>
#include <Logger/Logger.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <LogicalFileSystem/LogicalFolder.h>
#include <PhysicalFileSystem/PhysicalFileSystem.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/UIRenderItemBuilder.h>
#include <RenderSystem/IMaterialManager.h>
#include <assert.h>
#include <queue>
extern std::string testProjectPath;

Quad::EditorProjectManager *Quad::EditorProjectManager::GetInstance()
{
    static EditorProjectManager instance;

    return &instance;
}

Quad::EditorProjectManager::EditorProjectManager() {}

Quad::EditorProjectManager::~EditorProjectManager() {}

bool Quad::EditorProjectManager::Initialize()
{
    ProjectConfig *projectConfig = ProjectConfig::GetInstance();
    projectConfig->Load();

    EditorConfig *editorConfig = EditorConfig::GetInstance();
    const std::string editorRootPath = editorConfig->GetEditorRootPath();

    QuadLog::Logger *log = QuadLog::Logger::GetInstance();
    // bool logRet = log->SetLoggerFile("EditorLogFile.txt");

    mPhysicalFileSystem = QuadPF::PhysicalFileSystem::GetInstance();

    mLogicalFileSystem = std::make_unique<QuadLF::LogicalFileSystem>(mPhysicalFileSystem);
    mLogicalFileSystem->Initialize(testProjectPath);

    mAssetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();
    // mAssetLoader = CoreAsset::AssetLoader::GetInstance();

    mUIMaterialManager = std::make_unique<CoreAsset::UIMaterialManager>(Render::IMaterialManager::GetInstance());

    mUIRenderItemBuilder = std::make_unique<Render::UIRenderItemBuilder>(
        Render::IRenderSystem::GetInstance(), UI::UIManager::GetInstance(), GRM::IGpuResourceManager::GetInstance(),
        Render::AssetResolver::GetInstance());

    //.shader.buffer 파일을 읽어서 gpuBuffer를 gpuBufferContextSystem에 등록한다.
    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();

    gpuBufferContextSystem->LoadShaderBufferFile(editorRootPath + "/Shader/shaderbuffer.shader.buffer");

    mGpuSamplerSystem = std::make_unique<GRM::GpuSamplerSystem>(GRM::IGpuResourceManager::GetInstance());
    mGpuSamplerSystem->LoadShaderSamplerFile(editorRootPath + "/Shader/Sampler.sampler");

    mEditorShaderImporter = std::make_unique<EditorShaderImporter>(Render::IMaterialManager::GetInstance());

    EditorShaderImporter *editorShaderImporter = EditorShaderImporter::GetInstance();

    std::wstring buffer;
    buffer.resize(100);
    GetCurrentDirectoryW(100, buffer.data());

    editorShaderImporter->Import(editorRootPath + "/Shader/testshader.shader");
    editorShaderImporter->Import(editorRootPath + "/Shader/DefaultUIShader.shader");
    editorShaderImporter->Import(editorRootPath + "/Shader/DefaultUIResizeGizmoShader.shader");

    mTextureImporter = std::make_unique<EditorTextureImporter>(Import::TextureImporter::GetInstance(),
                                                               CoreAsset::TextureManager::GetInstance(),
                                                               CoreAsset::AssetMetaDataManager::GetInstance());

    mAssetRawFolderPath = ProjectConfig::GetInstance()->GetProjectPath();
    mAssetRawFolderPath += "/RawAsset";

    // 2025  07 20
    InitLogicalDirectoryStructure();

    LoadProjectAsset();

    return true;
}

void Quad::EditorProjectManager::SaveProject()
{
    SaveAsset();

    ProjectConfig *projectConfig = ProjectConfig::GetInstance();
    projectConfig->Save();
}

void Quad::EditorProjectManager::SaveAsset()
{

    // GlobalAssetRegistrySystem이 Asset List 를 유지하고있다
    CoreAsset::GlobalAssetRegistrySystem *globalAssetRegistrySystem =
        CoreAsset::GlobalAssetRegistrySystem::GetInstance();

    const std::vector<CoreAsset::AssetPtr> &dirtyAssetLists = globalAssetRegistrySystem->GetDirtyAssetList();

    // dirty list 를 순회하면서 메타데이터에 접근하여 , 파일경로를 얻고 저장한다.

    ProjectConfig *projectConfig = ProjectConfig::GetInstance();
    const std::string &projectRootPath = projectConfig->GetProjectPath();

    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();

    for (size_t i = 0; i < dirtyAssetLists.size(); ++i)
    {
        CoreAsset::Asset *asset = dirtyAssetLists[i].Get();

        if (asset == nullptr)
            continue;

        CoreAsset::AssetMetaData *assetMetaData = mAssetMetaDataManager->GetMetaData(asset->GetID());

        const std::string filePath =
            projectRootPath + "/" + assetMetaData->mFilePath + "." + CoreAsset::GetAssetFileExtension();

        bool ret = assetManager->StoreAsset(asset, filePath.c_str()); // 메타데이터만 저장됨

        // 이렇게 flag로 처리하거나 ,아니면 그냥 에셋storer별로 처리하게끔.
        if (assetMetaData->mKeepRawDataFlag)
        {
            // raw data도 존재한다면 , 변경되었다면 저장해야함
            // 메타데이터에 저장되는 raw파일경로는 사실 이름만 저장하는거지 , 실제 폴더경로는 고정된것
            std::string rawDataFilePath = mAssetRawFolderPath + "/" + assetMetaData->mRawFileName.c_str();
            ret = assetManager->StoreAssetRawData(asset, rawDataFilePath.c_str());
        }
    }
}

void Quad::EditorProjectManager::SaveMap() {}

void Quad::EditorProjectManager::InitLogicalDirectoryStructure()
{
    // 프로젝트의 논리적 폴더 구조 로드 테스트

    const std::string testLDSFileName = "testLDSFile";
    QuadRW::BinaryReader binaryReader;
    const std::string testLDSFilePath = testProjectPath + "/" + testLDSFileName + ".lds";
    bool ret = mLogicalFileSystem->LoadLogicalDirectoryStructureAsBinaryReader(binaryReader, testLDSFilePath);
}

void Quad::EditorProjectManager::LoadProjectAsset()
{
    EditorConfig *editorConfig = EditorConfig::GetInstance();
    const std::string editorRootPath = editorConfig->GetEditorRootPath();

    // SerializedAssetTContainer *serializedAssetContainer = SerializedAssetTContainer::GetInstance();

    QuadLF::LogicalFolder *assetFolder = mLogicalFileSystem->GetRootFolder();

    std::queue<QuadLF::LogicalFolder *> folderQueue;
    folderQueue.push(assetFolder);

    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();

    CoreAsset::AssetMetaDataManager *assetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();

    // 각 폴더내 asset파일들을 찾아서 Load한다.
    while (!folderQueue.empty())
    {

        QuadLF::LogicalFolder *folder = folderQueue.front();
        folderQueue.pop();

        std::vector<std::string> assetFileList;
        std::string physcialFolderPath = mLogicalFileSystem->GetPhysicalFullPath(folder);
        mPhysicalFileSystem->GetFileListByExtension(physcialFolderPath, CoreAsset::GetAssetFileExtension(),
                                                    assetFileList);

        for (auto &assetPath : assetFileList)
        {
            //.asset 로드
            assetPath = physcialFolderPath + "/" + assetPath;

            CoreAsset::Asset *asset = assetManager->LoadAsset(assetPath.c_str());
            CoreAsset::AssetMetaData *assetMetaData = assetMetaDataManager->GetMetaData(asset);

            // logical file generation
            std::string parentFolderLogicalPath = folder->GetFullPath();
            std::string logicalFilePath = parentFolderLogicalPath + "/" + asset->GetName().c_str();

            QuadLF::LogicalFileAssetInfo assetFileInfo;
            assetFileInfo.mAssetID = asset->GetID();
            assetFileInfo.mAssetType = asset->GetType();
            assetFileInfo.mName = asset->GetName().c_str();

            mLogicalFileSystem->MakeFile(assetFileInfo, asset->GetName().c_str(), folder);
        }
    }

    CreateAssetAndLogicalFile();
}

void Quad::EditorProjectManager::CreateAssetAndLogicalFile()
{
    // asset 생성

    // SerializedAssetTContainer *serializedAssetContainer = SerializedAssetTContainer::GetInstance();

    //// 2025 07 20
    // CreateTextureAndLogicalFile(serializedAssetContainer);

    //// material
    // CreateMaterialAndLogicalFile(serializedAssetContainer);

    // mesh
}

void Quad::EditorProjectManager::CreateTextureAndLogicalFile(SerializedAssetTContainer *serializedAssetContainer)
{
    // texture
    // CoreAsset::TextureManager *textureManager = CoreAsset::TextureManager::GetInstance();

    // const std::vector<SerializedAssetContainerContext> &serializedAssetContextVector =
    //     serializedAssetContainer->GetSerializedAssetContextVector(CoreAsset::EAssetType::eTexture);

    // for (const auto &context : serializedAssetContextVector)
    //{
    //     CoreAsset::SerializedTexture *textureData =
    //         static_cast<CoreAsset::SerializedTexture *>(context.mSerializedAsset.get());
    //     CoreAsset::SerializedTextureRawData *textureRawData =
    //         static_cast<CoreAsset::SerializedTextureRawData *>(context.mSerializedRawAssetData.get());

    //    // texture 생성
    //    CoreAsset::Texture *texture = textureManager->CreateTexture(*textureData, std::move(*textureRawData),
    //                                                                context.assetMetaData, context.mParentFolderPath);

    //    // logicalFile 생성
    //    QuadLF::LogicalFileAssetInfo logicalFileInfo;
    //    logicalFileInfo.mAssetID = textureData->mAssetID;
    //    logicalFileInfo.mAssetType = textureData->mAssetType;
    //    logicalFileInfo.mName = textureData->mAssetName;

    //    QuadLF::LogicalFolder *parentFolder = mLogicalFileSystem->GetFolder(context.mParentFolderPath);
    //    QuadLF::LogicalFile *logicalFile =
    //        mLogicalFileSystem->MakeFile(logicalFileInfo, logicalFileInfo.mName, parentFolder);

    //    mLogicalFileSystem->SetPhysicalBindingFlag(logicalFile, true);

    //    // asset meta Data등록
    //    mAssetMetaDataManager->Register(context.assetMetaData);
    //}
}

void Quad::EditorProjectManager::CreateMaterialAndLogicalFile(SerializedAssetTContainer *serializedAssetContainer)
{

    // CoreAsset::MaterialManager *materialManager = CoreAsset::MaterialManager::GetInstance();

    // const std::vector<SerializedAssetContainerContext> &serializedAssetContextVector =
    //     serializedAssetContainer->GetSerializedAssetContextVector(CoreAsset::EAssetType::eMaterial);

    // for (const auto &context : serializedAssetContextVector)
    //{

    //    CoreAsset::SerializedMaterial *materialData =
    //        static_cast<CoreAsset::SerializedMaterial *>(context.mSerializedAsset.get());

    //    // material 생성
    //    CoreAsset::Material *material =
    //        materialManager->CreateMaterial(*materialData, context.assetMetaData, context.mParentFolderPath);

    //    // logicalFile 생성
    //    QuadLF::LogicalFileAssetInfo logicalFileInfo;
    //    logicalFileInfo.mAssetID = materialData->mAssetID;
    //    logicalFileInfo.mAssetType = materialData->mAssetType;
    //    logicalFileInfo.mName = materialData->mAssetName;

    //    QuadLF::LogicalFolder *parentFolder = mLogicalFileSystem->GetFolder(context.mParentFolderPath);
    //    QuadLF::LogicalFile *logicalFile =
    //        mLogicalFileSystem->MakeFile(logicalFileInfo, logicalFileInfo.mName, parentFolder);

    //    mLogicalFileSystem->SetPhysicalBindingFlag(logicalFile, true);

    //    // asset meta Data등록
    //    mAssetMetaDataManager->Register(context.assetMetaData);
    //}
}
