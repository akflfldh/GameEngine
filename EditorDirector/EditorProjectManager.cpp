#include "EditorDirector/EditorProjectManager.h"

#include "EditorDirector/EditorShaderImporter.h"
#include "EditorDirector/EditorTextureImporter.h"
#include "EditorDirector/SerializedAssetContainer.h"
#include <BinaryReaderWriter/BinaryReader.h>
#include <Core/GpuBufferContextSystem.h>
#include <Core/GpuSamplerSystem.h>
#include <Core/ProjectConfig.h>
#include <Core/UIRenderItemBuilder.h>
#include <CoreAsset/AssetCommon.h>
#include <CoreAsset/AssetLoader.h>
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
    mAssetLoader = CoreAsset::AssetLoader::GetInstance();

    mUIMaterialManager = std::make_unique<CoreAsset::UIMaterialManager>(Render::IMaterialManager::GetInstance());

    mUIRenderItemBuilder = std::make_unique<UIRenderItemBuilder>(
        Render::IRenderSystem::GetInstance(), UI::UIManager::GetInstance(), GRM::IGpuResourceManager::GetInstance());

    //.shader.buffer 파일을 읽어서 gpuBuffer를 gpuBufferContextSystem에 등록한다.
    Core::GpuBufferContextSystem *gpuBufferContextSystem = Core::GpuBufferContextSystem::GetInstance();

    gpuBufferContextSystem->LoadShaderBufferFile(editorRootPath + "/Shader/shaderbuffer.shader.buffer");

    mGpuSamplerSystem = std::make_unique<Core::GpuSamplerSystem>(GRM::IGpuResourceManager::GetInstance());
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

    // 2025  07 20
    InitLogicalDirectoryStructure();

    if (true)
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
    // asset들중 dirty flag 가 켜져있는 asset들에대해서 저장

    CoreAsset::GlobalAssetRegistrySystem *globalAssetRegistrySystem =
        CoreAsset::GlobalAssetRegistrySystem::GetInstance();
    const std::vector<CoreAsset::AssetPtr<CoreAsset::Asset>> &dirtyAssetList =
        globalAssetRegistrySystem->GetDirtyAssetList();

    // 저장할게없다.
    if (dirtyAssetList.empty())
        return;

    ProjectConfig *projectConfig = ProjectConfig::GetInstance();
    const std::string projectFolderPath = projectConfig->GetProjectPath();

    // 파일경로는 asset폴더의 물리적경로 + asset의 논리적경로
    QuadLF::LogicalFileSystem *logicalFileSystem = QuadLF::LogicalFileSystem::GetInstance();

    CoreAsset::MaterialStorer *materialStorer = CoreAsset::MaterialStorer::GetInstance();

    for (const auto &pAssetElement : dirtyAssetList)
    {
        CoreAsset::Asset *asset = pAssetElement.Get();

        if (asset == nullptr)
            continue;

        CoreAsset::AssetMetaData *assetMetaData = mAssetMetaDataManager->GetMetaData(asset->GetID());

        const std::string filePath =
            projectFolderPath + "/" + assetMetaData->mFilePath + "." + CoreAsset::GetAssetFileExtension();

        bool ret = false;
        switch (asset->GetType())
        {
        case CoreAsset::EAssetType::eTexture:
        {

            CoreAsset::Texture *texture = static_cast<CoreAsset::Texture *>(asset);
        }
        break;
        case CoreAsset::EAssetType::eMaterial:
        {
            CoreAsset::Material *material = static_cast<CoreAsset::Material *>(asset);

            ret = materialStorer->Store(material, assetMetaData, filePath);
        }
        break;
        }
    }

    // 각 asset storer에게 요청

    // 그리고 dirty flag  를 false 설정
}

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

    SerializedAssetTContainer *serializedAssetContainer = SerializedAssetTContainer::GetInstance();

    QuadLF::LogicalFolder *assetFolder = mLogicalFileSystem->GetRootFolder();

    std::queue<QuadLF::LogicalFolder *> folderQueue;
    folderQueue.push(assetFolder);

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
            CoreAsset::AssetMetaData assetMetaData;
            std::unique_ptr<CoreAsset::SerializedAsset> assetData = mAssetLoader->LoadAsset(assetPath, assetMetaData);

            // rawData load
            // rawDataFile path

            std::string parentFolderLogicalPath = folder->GetFullPath();
            std::string logicalFilePath = parentFolderLogicalPath + "/" + assetData->mAssetName;
            std::string rawDataFilePath = editorRootPath + "/" + CoreAsset::GetAssetRawFileName(logicalFilePath);

            //.raw로드
            std::unique_ptr<CoreAsset::SerializedAssetRawData> assetRawData =
                mAssetLoader->LoadAssetRawData(rawDataFilePath, assetData->mAssetType);

            // table에 타입별로 보관
            serializedAssetContainer->Register(std::move(assetData), std::move(assetRawData), assetMetaData,
                                               parentFolderLogicalPath);
        }
    }

    CreateAssetAndLogicalFile();

    serializedAssetContainer->Clear();
}

void Quad::EditorProjectManager::CreateAssetAndLogicalFile()
{
    // asset 생성

    SerializedAssetTContainer *serializedAssetContainer = SerializedAssetTContainer::GetInstance();

    // 2025 07 20
    CreateTextureAndLogicalFile(serializedAssetContainer);

    // material
    CreateMaterialAndLogicalFile(serializedAssetContainer);

    // mesh
}

void Quad::EditorProjectManager::CreateTextureAndLogicalFile(SerializedAssetTContainer *serializedAssetContainer)
{
    // texture
    CoreAsset::TextureManager *textureManager = CoreAsset::TextureManager::GetInstance();

    const std::vector<SerializedAssetContainerContext> &serializedAssetContextVector =
        serializedAssetContainer->GetSerializedAssetContextVector(CoreAsset::EAssetType::eTexture);

    for (const auto &context : serializedAssetContextVector)
    {
        CoreAsset::SerializedTexture *textureData =
            static_cast<CoreAsset::SerializedTexture *>(context.mSerializedAsset.get());
        CoreAsset::SerializedTextureRawData *textureRawData =
            static_cast<CoreAsset::SerializedTextureRawData *>(context.mSerializedRawAssetData.get());

        // texture 생성
        CoreAsset::Texture *texture = textureManager->CreateTexture(*textureData, std::move(*textureRawData),
                                                                    context.assetMetaData, context.mParentFolderPath);

        // logicalFile 생성
        QuadLF::LogicalFileAssetInfo logicalFileInfo;
        logicalFileInfo.mAssetID = textureData->mAssetID;
        logicalFileInfo.mAssetType = textureData->mAssetType;
        logicalFileInfo.mName = textureData->mAssetName;

        QuadLF::LogicalFolder *parentFolder = mLogicalFileSystem->GetFolder(context.mParentFolderPath);
        QuadLF::LogicalFile *logicalFile =
            mLogicalFileSystem->MakeFile(logicalFileInfo, logicalFileInfo.mName, parentFolder);

        mLogicalFileSystem->SetPhysicalBindingFlag(logicalFile, true);

        // asset meta Data등록
        mAssetMetaDataManager->Register(context.assetMetaData);
    }
}

void Quad::EditorProjectManager::CreateMaterialAndLogicalFile(SerializedAssetTContainer *serializedAssetContainer)
{

    CoreAsset::MaterialManager *materialManager = CoreAsset::MaterialManager::GetInstance();

    const std::vector<SerializedAssetContainerContext> &serializedAssetContextVector =
        serializedAssetContainer->GetSerializedAssetContextVector(CoreAsset::EAssetType::eMaterial);

    for (const auto &context : serializedAssetContextVector)
    {

        CoreAsset::SerializedMaterial *materialData =
            static_cast<CoreAsset::SerializedMaterial *>(context.mSerializedAsset.get());

        // material 생성
        CoreAsset::Material *material =
            materialManager->CreateMaterial(*materialData, context.assetMetaData, context.mParentFolderPath);

        // logicalFile 생성
        QuadLF::LogicalFileAssetInfo logicalFileInfo;
        logicalFileInfo.mAssetID = materialData->mAssetID;
        logicalFileInfo.mAssetType = materialData->mAssetType;
        logicalFileInfo.mName = materialData->mAssetName;

        QuadLF::LogicalFolder *parentFolder = mLogicalFileSystem->GetFolder(context.mParentFolderPath);
        QuadLF::LogicalFile *logicalFile =
            mLogicalFileSystem->MakeFile(logicalFileInfo, logicalFileInfo.mName, parentFolder);

        mLogicalFileSystem->SetPhysicalBindingFlag(logicalFile, true);

        // asset meta Data등록
        mAssetMetaDataManager->Register(context.assetMetaData);
    }
}
