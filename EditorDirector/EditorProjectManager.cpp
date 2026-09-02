#include "EditorDirector/EditorProjectManager.h"

#include "EditorDirector/EditorShaderImporter.h"
#include "EditorDirector/EditorTextureImporter.h"
#include "EditorDirector/SerializedAssetContainer.h"
#include <BinaryReaderWriter/BinaryReader.h>
#include <Core/ProjectConfig.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/GpuSamplerSystem.h>

#include <Core/CameraComponent.h>
#include <Core/CameraObject.h>
#include <Core/Entity.h>
#include <Core/IRenderProxyManager.h>
#include <Core/Map.h>
#include <Core/MaterialSystem.h>
#include <Core/StaticMeshComponent.h>
#include <Core/StaticMeshObject.h>
#include <Core/World.h>
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
#include <CoreBase/CoreAssert.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <EditorDirector/EditorConfig.h>
#include <EditorDirector/EditorMode.h>
#include <EditorDirector/EditorSceneManager.h>
#include <EditorDirector/EditorSelectionManager.h>
#include <EditorEditMode.h>
#include <EditorSceneController.h>
#include <GlobalOverlayManager.h>
#include <ImportModule/TextureImporter.h>
#include <Logger/Logger.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <LogicalFileSystem/LogicalFolder.h>
#include <PhysicalFileSystem/PhysicalFileSystem.h>
#include <ReflectSystem/ReflectionClassInfo.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/ObjectRenderItemBuilder.h>
#include <RenderFrontend/RenderFrontendType.h>
#include <RenderFrontend/UIRenderItemBuilder.h>
#include <RenderSystem/IMaterialManager.h>
#include <Utility/Utility.h>
#include <assert.h>
#include <queue>

// extern std::string testProjectPath;

Quad::EditorProjectManager *Quad::EditorProjectManager::GetInstance()
{
    static EditorProjectManager instance;

    return &instance;
}

Quad::EditorProjectManager::EditorProjectManager() : mUserProjectDLLHandle(nullptr)
{

    mEditorMode = std::make_unique<EditorEditMode>();
    Quad::EditorSceneManager::GetInstance()->GetUserWorld()->Register(mEditorMode->GetEditorMap());
    mEditorMode->SetShowDebugCollider(true);
}

Quad::EditorProjectManager::~EditorProjectManager() {}

bool Quad::EditorProjectManager::Initialize()
{
    bool ret = true;

    ProjectConfig *projectConfig = ProjectConfig::GetInstance();
    projectConfig->Load();

    mAssetRawFolderPath = ProjectConfig::GetInstance()->GetProjectPath();
    mAssetRawFolderPath = mAssetRawFolderPath / "RawAsset";

    InitSystems();
    CreateEditorAsset();
    LoadEditorAsset();

    mOnOpendMapCallbackSystem.Register(
        [](Map *map)
        {
            if (map)
            {
                map->mObjectRemovedCallbackSystem.Register(
                    [](Object *object) { EditorSelectionManager::GetInstance()->OnMapObjectRemoved(object); });
            }
        });

    return true;
}

bool Quad::EditorProjectManager::InitProject()
{
    LoadProject();

    return true;
}

void Quad::EditorProjectManager::InitEditorMap() {}

void Quad::EditorProjectManager::SaveProject()
{
    SaveAsset();

    SaveLogicalDirectoryStructure();

    ProjectConfig *projectConfig = ProjectConfig::GetInstance();
    projectConfig->Save();
}

void Quad::EditorProjectManager::OpenMap(Map *map)
{

    EditorSceneManager *sceneManager = EditorSceneManager::GetInstance();
    World *userWorld = sceneManager->GetUserWorld();

    Map *currentMap = userWorld->GetCurrentMap();

    auto loadNextMapNextCallback = [this, userWorld, map]()
    {
        auto assetManager = CoreAsset::AssetManager::GetInstance();

        // 이건 동기가 필요하니 , 메인스레드에서 그냥 수행
        bool bLoadedMapRawData = assetManager->LoadAssetRawData(map);

        userWorld->SetCurrentMap(map);

        // 에디터에서 새맵을 열면은반응해야하는 작업들이 많이있다.

        CreateEditorObjects(map, EditorSelectionManager::GetInstance());

        mOnOpendMapCallbackSystem.ExecuteCallbacks(map);
    };

    if (currentMap)
    {
        // 일단 Map의 더티플래그  + Undo ,Redo는 이후에 도입
        // 지금은항상 저장여부를 묻는 UI띄우기
        // 기존 맵의 저장여부를 묻는  box를 띄울거다 .
        GlobalOverlayManager::GlobalOverlayManager::GetInstance()->ShowSaveMapBox(currentMap, loadNextMapNextCallback);
    }
    else
    {
        loadNextMapNextCallback();
    }
}

void Quad::EditorProjectManager::SaveMap(Map *map)
{

    // 에디터 맵 저장 전 해야할작업
    // PreTask
    if (map == nullptr)
        return;

    // 맵 저장
    InternalSaveSingleAsset(map);
}

void Quad::EditorProjectManager::InitSystems()
{
    LoadUserProjectDLL();
}

void Quad::EditorProjectManager::SaveAsset()
{

    // GlobalAssetRegistrySystem이 Asset List 를 유지하고있다
    CoreAsset::GlobalAssetRegistrySystem *globalAssetRegistrySystem =
        CoreAsset::GlobalAssetRegistrySystem::GetInstance();

    const std::vector<CoreAsset::AssetPtr> &dirtyAssetLists = globalAssetRegistrySystem->GetDirtyAssetList();

    // dirty list 를 순회하면서 메타데이터에 접근하여 , 파일경로를 얻고 저장한다.

    for (size_t i = 0; i < dirtyAssetLists.size(); ++i)
    {
        CoreAsset::Asset *asset = dirtyAssetLists[i].Get();

        if ((asset == nullptr))
            continue;

        if (asset->GetType() == CoreAsset::EAssetType::eMap)
        {
            SaveMap(static_cast<Map *>(asset));
        }
        else
        {
            bool ret = InternalSaveSingleAsset(asset);
            // 저장실패 리스트를 모아서 로그출력할수도있다.
        }
    }

    globalAssetRegistrySystem->ClearDirtyAssetList();
}

void Quad::EditorProjectManager::SaveMap() {}

void Quad::EditorProjectManager::SaveLogicalDirectoryStructure()
{
    QuadLF::LogicalFileSystem *logicalFileSystem = QuadLF::LogicalFileSystem::GetInstance();

    QuadRW::BinaryWriter binaryWriter;
    const std::string testLDSFileName = "testLDSFile";

    const std::filesystem::path &testLDSFilePath = EditorConfig::GetInstance()->GetEditorRootPath();
    //    testProjectPath;

    logicalFileSystem->SaveLogicalDirectoryStructureAsBinaryWriter(binaryWriter, testLDSFilePath, testLDSFileName);
}

void Quad::EditorProjectManager::LoadProject()
{

    InitLogicalDirectoryStructure();
    LoadProjectAsset();
    LoadProjectCXXList();
}

bool Quad::EditorProjectManager::InternalSaveSingleAsset(CoreAsset::Asset *asset)
{

    CoreAsset::AssetMetaDataManager *assetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();

    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();

    ProjectConfig *projectConfig = ProjectConfig::GetInstance();
    const std::filesystem::path &projectRootPath = projectConfig->GetProjectPath();

    // assetMetaData를 가져온다.
    CoreAsset::AssetMetaData *assetMetaData = assetMetaDataManager->GetMetaData(asset->GetID());

    // 실제 파일경로 계산
    const std::filesystem::path filePath =
        projectRootPath / (assetMetaData->mFilePath + CoreAsset::GetAssetDotFileExtension());

    bool ret = assetManager->StoreAsset(asset, filePath, assetMetaData); // 메타데이터만 저장됨

    CHECK(ret, "AssetStore fail");

    if (asset->GetRawDataDirty())
    {
        // raw data도 존재한다면 , 변경되었다면 저장해야함
        // 메타데이터에 저장되는 raw파일경로는 사실 이름만 저장하는거지 , 실제 폴더경로는 고정된것
        std::filesystem::path rawDataFilePath = mAssetRawFolderPath / assetMetaData->mRawFileName.c_str();
        bool raw_ret = assetManager->StoreAssetRawData(asset, rawDataFilePath.c_str());
        if (raw_ret)
            asset->SetRawDataDirty(false);

        ret = ret && raw_ret;
    }

    return ret;
}

void Quad::EditorProjectManager::LoadUserProjectDLL()
{
    const std::filesystem::path &projectPath = ProjectConfig::GetInstance()->GetProjectPath();
    std::string projectName = ProjectConfig::GetInstance()->GetProjectName();
    std::filesystem::path DLLPath = projectPath / "build/Debug" / (projectName + ".dll");
    mUserProjectDLLHandle = CoreUtility::LibraryUtility::Load(DLLPath);
}

void Quad::EditorProjectManager::InitLogicalDirectoryStructure()
{
    // 프로젝트의 논리적 폴더 구조 로드 테스트

    QuadLF::LogicalFileSystem *logicalFileSystem = QuadLF::LogicalFileSystem::GetInstance();

    const std::string testLDSFileName = "testLDSFile";
    QuadRW::BinaryReader binaryReader;

    const std::filesystem::path testLDSFilePath =
        EditorConfig::GetInstance()->GetEditorRootPath() / (testLDSFileName + ".lds");
    bool ret = logicalFileSystem->LoadLogicalDirectoryStructureAsBinaryReader(binaryReader, testLDSFilePath);
}

void Quad::EditorProjectManager::LoadProjectAsset()
{
    QuadLF::LogicalFileSystem *logicalFileSystem = QuadLF::LogicalFileSystem::GetInstance();
    QuadPF::PhysicalFileSystem *pyhsicalFileSystem = QuadPF::PhysicalFileSystem::GetInstance();

    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();
    CoreAsset::AssetMetaDataManager *assetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();

    EditorConfig *editorConfig = EditorConfig::GetInstance();
    const std::filesystem::path &editorRootPath = editorConfig->GetEditorRootPath();

    QuadLF::LogicalFolder *assetFolder = logicalFileSystem->GetAssetFolder();

    std::queue<QuadLF::LogicalFolder *> folderQueue;
    folderQueue.push(assetFolder);

    // 각 폴더내 asset파일들을 찾아서 Load한다.

    Map *loadedDefaultMap = nullptr;
    World *userWorld = EditorSceneManager::GetInstance()->GetUserWorld();
    userWorld->SetEngineMode(mEditorMode.get());

    CoreAsset::AssetLoadExecutionContext assetLoadExecutionContext;
    assetLoadExecutionContext.mIgnoredAssetType = CoreAsset::EAssetType::ePrefab;
    std::vector<std::tuple<std::filesystem::path, std::string, QuadLF::LogicalFolder *>> ignoredPrefabPathContextList;

    auto loadFromPathLamda = [&assetLoadExecutionContext, &ignoredPrefabPathContextList, assetManager,
                              assetMetaDataManager, logicalFileSystem, userWorld, loadedDefaultMap = &loadedDefaultMap](
                                 const std::filesystem::path &assetPath, const std::string &parentLogicalFolderPath,
                                 QuadLF::LogicalFolder *folder)
    {
        CoreAsset::AssetLoadResult assetLoadResult =
            assetManager->LoadAsset(assetPath.c_str(), parentLogicalFolderPath, assetLoadExecutionContext);

        CoreAsset::Asset *asset = assetLoadResult.pAsset;

        if (assetLoadResult.mResultFlag == CoreAsset::EAssetLoadResultFlag::eIgnore)
        {
            ignoredPrefabPathContextList.push_back({assetPath, parentLogicalFolderPath, folder});
            return;
        }

        if (assetLoadResult.mResultFlag != CoreAsset::EAssetLoadResultFlag::eSuccess)
        {

            return;
        }

        CoreAsset::AssetMetaData *assetMetaData = assetMetaDataManager->GetMetaData(asset);

        // logical file generation
        std::string logicalFilePath = parentLogicalFolderPath + "/" + asset->GetName().c_str();

        QuadLF::LogicalFileAssetInfo assetFileInfo;
        assetFileInfo.mAssetID = asset->GetID();
        assetFileInfo.mAssetType = asset->GetType();
        assetFileInfo.mName = asset->GetName().c_str();

        logicalFileSystem->MakeFile(assetFileInfo, asset->GetName().c_str(), folder);
        assetMetaData->mFilePath = logicalFilePath;

        if (asset->GetType() == CoreAsset::EAssetType::eMap)
        {
            Map *map = static_cast<Map *>(asset);
            userWorld->Register(map);

            if (*loadedDefaultMap == nullptr)
            {
                *loadedDefaultMap = map;
            }
        }
    };

    while (!folderQueue.empty())
    {

        QuadLF::LogicalFolder *folder = folderQueue.front();
        folderQueue.pop();

        std::vector<std::string> assetFileList;
        std::filesystem::path physcialFolderPath = logicalFileSystem->GetPhysicalFullPath(folder);
        pyhsicalFileSystem->GetFileListByExtension(physcialFolderPath, CoreAsset::GetAssetFileExtension(),
                                                   assetFileList);

        std::string parentLogicalFolderPath = folder->GetFullPath();

        for (auto &assetName : assetFileList)
        {

            //.asset 로드
            std::filesystem::path assetPath = physcialFolderPath / assetName;

            loadFromPathLamda(assetPath, parentLogicalFolderPath, folder);

            // CoreAsset::AssetLoadResult assetLoadResult =
            //     assetManager->LoadAsset(assetPath.c_str(), parentLogicalFolderPath, assetLoadExecutionContext);

            // CoreAsset::Asset *asset = assetLoadResult.pAsset;

            // if (assetLoadResult.mResultFlag == CoreAsset::EAssetLoadResultFlag::eIgnore)
            //{
            //     ignoredPrefabPathList.push_back({assetPath, parentLogicalFolderPath});
            //     continue;
            // }

            // if (assetLoadResult.mResultFlag != CoreAsset::EAssetLoadResultFlag::eSuccess)
            //{

            //    continue;
            //}

            // CoreAsset::AssetMetaData *assetMetaData = assetMetaDataManager->GetMetaData(asset);

            //// logical file generation
            // std::string logicalFilePath = parentLogicalFolderPath + "/" + asset->GetName().c_str();

            // QuadLF::LogicalFileAssetInfo assetFileInfo;
            // assetFileInfo.mAssetID = asset->GetID();
            // assetFileInfo.mAssetType = asset->GetType();
            // assetFileInfo.mName = asset->GetName().c_str();

            // logicalFileSystem->MakeFile(assetFileInfo, asset->GetName().c_str(), folder);
            // assetMetaData->mFilePath = logicalFilePath;

            // if (asset->GetType() == CoreAsset::EAssetType::eMap)
            //{
            //     Map *map = static_cast<Map *>(asset);
            //     userWorld->Register(map);

            //    if (loadedDefaultMap == nullptr)
            //    {
            //        loadedDefaultMap = map;
            //    }
            //}
        }

        for (auto childFolder : folder->GetChildNodeList())
        {
            if (childFolder->GetNodeType() == QuadLF::ELogicalNodeType::eFolder)
            {
                folderQueue.push(static_cast<QuadLF::LogicalFolder *>(childFolder));
            }
        }
    }

    // 미루었던 prefab load

    for (const auto &e : ignoredPrefabPathContextList)
    {
        assetLoadExecutionContext.mIgnoredAssetType = CoreAsset::EAssetType::eUnknown;
        loadFromPathLamda(std::get<0>(e), std::get<1>(e), std::get<2>(e));
    }

    // map이 하나도 없었다면
    // map을 생성해주자.
    if (loadedDefaultMap == nullptr)
    {
        CreateDefaultUserMap();
    }
    else
    {
        OpenMap(loadedDefaultMap);
    }
}

void Quad::EditorProjectManager::CreateEditorAsset()
{
    EditorConfig *editorConfig = EditorConfig::GetInstance();
    const std::filesystem::path &editorRootPath = editorConfig->GetEditorRootPath();
    const std::filesystem::path editorShaderPath = editorRootPath / "Shader";

    const std::filesystem::path GizmoMaterialHLSLPath = editorShaderPath / "GizmoMesh.hlsl";

    std::vector<uint8_t> shaderBuffer;
    /*  std::ifstream fin(GizmoMaterialHLSLPath.c_str(), std::ios::binary);
      CHECK(fin.is_open() == true);

      fin.seekg(0, std::ios_base::end);
      size_t fileSize = fin.tellg();
      fin.seekg(0, std::ios_base::beg);
              std::vector<uint8_t> shaderBuffer(fileSize);

      fin.read((char *)shaderBuffer.data(), fileSize);

      fin.close();*/

    auto pysicalFileSystem = QuadPF::PhysicalFileSystem::GetInstance();
    if (!pysicalFileSystem->ReadFileToBuffer(GizmoMaterialHLSLPath, shaderBuffer))
    {
        // error 출력
        return;
    }

    auto assetManager = CoreAsset::AssetManager::GetInstance();
    auto materialSystem = Core::MaterialSystem::GetInstance();

    // gizmo material
    CoreAsset::IntermediateMaterial intermediateGizmoMat;
    intermediateGizmoMat.mAssetName = "GizmoMaterial";
    intermediateGizmoMat.mAssetType = CoreAsset::EAssetType::eMaterial;
    intermediateGizmoMat.mGpuMaterialID = 10;

    CoreAsset::Material *gizmoMaterial = static_cast<CoreAsset::Material *>(
        assetManager->CreateAsset(CoreAsset::EAssetType::eMaterial, intermediateGizmoMat, "Engine", true).Get());

    gizmoMaterial->SetUseExplicitGpuMaterial(true);

    Render::MaterialRenderSettingInfo gizmoRenderSettingInfo;
    gizmoRenderSettingInfo.mCullMode = Render::ECullMode::eNone;
    gizmoRenderSettingInfo.mFillMode = Render::EFillMode::eSolidMode;
    gizmoRenderSettingInfo.mCCW = 0;
    gizmoRenderSettingInfo.mDepthCompareMode = Render::EDepthStencilCompareMode::eLess;
    gizmoRenderSettingInfo.mBlendMode = Render::EBlendMode::eOpaque;
    gizmoRenderSettingInfo.mDepthWriteMode = Render::EDepthWriteMode::eEnabled;

    materialSystem->BuildGpuMaterialDirectly(gizmoMaterial, shaderBuffer.data(), shaderBuffer.size(),
                                             gizmoRenderSettingInfo);

    const std::filesystem::path DebugColliderMaterialHLSLPath = editorShaderPath / "DebugCollider.hlsl";
    // fin.open(DebugColliderMaterialHLSLPath.c_str(), std::ios::binary);

    // CHECK(fin.is_open() == true);
    // fin.seekg(0, std::ios_base::end);
    // fileSize = fin.tellg();
    // fin.seekg(0, std::ios_base::beg);
    // shaderBuffer.resize(fileSize);
    // fin.read((char *)shaderBuffer.data(), fileSize);

    // fin.close();

    if (!pysicalFileSystem->ReadFileToBuffer(DebugColliderMaterialHLSLPath, shaderBuffer))
    {
        // error 출력
        return;
    }

    // default collider material
    CoreAsset::IntermediateMaterial intermediateMat;
    intermediateMat.mAssetName = "DefaultColliderMaterial";
    intermediateMat.mAssetType = CoreAsset::EAssetType::eMaterial;
    intermediateMat.mGpuMaterialID = 11;

    CoreAsset::Material *defaultColliderMat = static_cast<CoreAsset::Material *>(
        assetManager->CreateAsset(CoreAsset::EAssetType::eMaterial, intermediateMat, "Engine", true).Get());

    defaultColliderMat->SetUseExplicitGpuMaterial(true);

    Render::MaterialRenderSettingInfo defaultColliderRenderSettingInfo;
    defaultColliderRenderSettingInfo.mCullMode = Render::ECullMode::eBack;
    defaultColliderRenderSettingInfo.mFillMode = Render::EFillMode::eWireFrameMode;
    defaultColliderRenderSettingInfo.mCCW = 0;
    defaultColliderRenderSettingInfo.mDepthCompareMode = Render::EDepthStencilCompareMode::eLess;
    defaultColliderRenderSettingInfo.mBlendMode = Render::EBlendMode::eOpaque;
    defaultColliderRenderSettingInfo.mDepthWriteMode = Render::EDepthWriteMode::eEnabled;

    materialSystem->BuildGpuMaterialDirectly(defaultColliderMat, shaderBuffer.data(), shaderBuffer.size(),
                                             defaultColliderRenderSettingInfo);
}

void Quad::EditorProjectManager::LoadEditorAsset() {}

void Quad::EditorProjectManager::LoadProjectCXXList()
{

    auto projectConfig = Quad::ProjectConfig::GetInstance();
    const std::filesystem::path &projectPath = projectConfig->GetProjectPath();
    std::filesystem::path cxxListPath = projectPath / "CXXList.txt";

    auto reflectSystem = ReflectionSystem::GetInstance();
    std::vector<Quad::ClassInfo *> userClassList = reflectSystem->GetUserClassList();

    auto logicalFileSystem = QuadLF::LogicalFileSystem::GetInstance();
    auto cxxFolder = logicalFileSystem->GetCXXFolder();

    for (Quad::ClassInfo *c : userClassList)
    {
        QuadLF::LogicalFileAssetInfo info;
        info.mAssetID = 0;
        info.mAssetType = CoreAsset::EAssetType::eCXX;
        info.mName = c->GetTypeName();
        logicalFileSystem->MakeFile(info, c->GetTypeName(), cxxFolder);
    }
}

void Quad::EditorProjectManager::CreateDefaultUserMap()
{
    QuadLF::LogicalFileSystem *logicalFileSystem = QuadLF::LogicalFileSystem::GetInstance();

    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();
    World *userWorld = EditorSceneManager::GetInstance()->GetUserWorld();

    // 1. 메모리에 새로운 Map 에셋 생성
    Core::IntermediateMap intermediateMap;
    intermediateMap.mAssetName = "DefaultMap";
    intermediateMap.mAssetType = CoreAsset::EAssetType::eMap;

    CoreAsset::AssetPtr pMap = assetManager->CreateAsset(CoreAsset::EAssetType::eMap, intermediateMap, "/Asset");
    Map *defaultMap = static_cast<Map *>(pMap.Get());

    CameraObject *camObject = static_cast<CameraObject *>(defaultMap->CreateEntity<CameraObject>("MainCamera"));

    camObject->GetCameraComponent()->SetPositionLocal({0, 0, -10});
    camObject->GetCameraComponent()->SetFar(10025.5f);
    userWorld->Register(defaultMap);
    userWorld->SetCurrentMap(defaultMap);

    QuadLF::LogicalFileAssetInfo assetFileInfo;
    assetFileInfo.mAssetID = defaultMap->GetID();
    assetFileInfo.mAssetType = defaultMap->GetType();
    assetFileInfo.mName = defaultMap->GetName().c_str();
    QuadLF::LogicalFolder *rootAssetFolder = logicalFileSystem->GetAssetFolder();

    auto file = logicalFileSystem->MakeFile(assetFileInfo, defaultMap->GetName().c_str(), rootAssetFolder);

    auto assetMetaData = CoreAsset::AssetMetaDataManager::GetInstance()->GetMetaData(defaultMap);
    assetMetaData->mFilePath = file->GetFullPath();

    camObject->SetTestValue(2512.0f);

    Core::StaticMeshObject *cube = defaultMap->CreateEntity<Core::StaticMeshObject>("Cube");
    cube->mStaticMeshComponent->SetMesh("Engine/Cube");
    cube->SetPositionWorld(0, 0, 10);
    cube->SetScaleWorld(20, 20, 20);

    SaveMap(defaultMap);
    ProjectConfig *projectConfig = ProjectConfig::GetInstance();
    projectConfig->Save();
}

void Quad::EditorProjectManager::CreateEditorObjects(Map *map, BaseSelectionManager *selectionManager)
{

    // 그것을 다해줘야함 콜백으로 등록하게끔
    CameraObject *camObject = static_cast<CameraObject *>(map->CreateEngineEntity<CameraObject>("EdtorCamera"));
    if (camObject)
    {

        camObject->GetCameraComponent()->SetPositionLocal({0, 0, -10});
        camObject->GetCameraComponent()->SetFar(10025.5f);
    }

    auto editorCameraController = map->CreateEngineEntity<EditorSceneController>("EditorCameraController");
    if (editorCameraController && camObject)
    {
        editorCameraController->Possess(camObject);
    }
    editorCameraController->Intialize(selectionManager);

    // userWorld->StartMap();
    map->GetWorld()->StartMap();

    EditorEditMode *editorMode = static_cast<EditorEditMode *>(map->GetWorld()->GetEngineMode());
    editorMode->SetEditorController(editorCameraController);

    if (camObject)
        editorMode->SetEditorCameraComponent(camObject->GetCameraComponent());

    editorMode->BindSourceMapToVisualizerManager(map);

    map->ClearDirty();
    map->SetRawDataDirty(false);
}

void Quad::EditorProjectManager::SetDrawDebugColliderFlag(bool flag)
{

    if (mEditorMode)
    {
        mEditorMode->SetShowDebugCollider(flag);
    }
}

bool Quad::EditorProjectManager::GetDrawDebugColliderFlag() const
{
    if (mEditorMode)
    {
        return mEditorMode->GetShowDebugCollider();
    }

    return false;
}

void Quad::EditorProjectManager::SaveAsset(CoreAsset::Asset *asset)
{

    InternalSaveSingleAsset(asset);
}
