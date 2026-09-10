#include "GameDirector.h"

#include "Application.h"
#include "GameDirector/GameWindowController.h"
#include <Core/LogicalWindow.h>
#include <Core/MapFactory.h>
#include <Core/MapLoader.h>
#include <Core/MapStorer.h>
#include <Core/PrefabFactory.h>
#include <Core/PrefabLoader.h>
#include <Core/PrefabStorer.h>
#include <Core/World.h>
#include <CoreAsset/AssetFactoryManager.h>
#include <CoreAsset/AssetIOManager.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/FontFactory.h>
#include <CoreAsset/MaterialFactory.h>
#include <CoreAsset/MaterialLoader.h>
#include <CoreAsset/MaterialStorer.h>
#include <CoreAsset/MeshFactory.h>
#include <CoreAsset/MeshLoader.h>
#include <CoreAsset/MeshStorer.h>
#include <CoreAsset/TextureFactory.h>
#include <CoreAsset/TextureLoader.h>
#include <CoreAsset/TextureStorer.h>
#include <CoreBase/BinaryArch.h>
#include <GameDirector/GameRuntimeConfig.h>
#include <GameDirector/GameRuntimeMode.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/ObjectRenderItemBuilder.h>
#include <RenderFrontend/RenderPipelineManager.h>
#include <Utility/Utility.h>

#include <Core/Map.h>
#include <CoreAsset/PakAssetDataSource.h>
#include <CoreAsset/UIMaterialManager.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/GpuSamplerSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <PhysicalFileSystem/PhysicalFileSystem.h>
#include <RenderFrontend/UIRenderItemBuilder.h>
#include <RenderSystem/IMaterialManager.h>

Quad::GameDirector *Quad::GameDirector::GetInstance()
{

    static GameDirector instance;
    return &instance;
}

Quad::GameDirector::GameDirector()
    : mGameWindowController(std::make_unique<GameWindowController>()),
      mGameRuntimeMode(std::make_unique<GameRuntimeMode>()), mWorld(std::make_unique<World>())

{
}

Quad::GameDirector::~GameDirector() {}

void Quad::GameDirector::InitRuntimeConfig(const std::filesystem::path &gameRootPath)
{

    mGameRuntimeConfig = std::make_unique<GameRuntimeConfig>(gameRootPath);
}

void Quad::GameDirector::Initialize()
{

    if (!LoadGameBuildManifest())
    {
        return;
    }

    // game window create, init
    // window not visible
    mApp = Quad::Application::GetInstance();
    mRenderPipelineManager = Render::RenderPipelineManager::GetInstance();
    mGameWindowController->Initialize(*mRenderPipelineManager);

    // asset system

    CoreAsset::AssetFactoryManager *assetFactoryManager = CoreAsset::AssetFactoryManager::GetInstance();
    // CoreAsset::AssetImporterManager *assetImporterManager = CoreAsset::AssetImporterManager::GetInstance();
    CoreAsset::AssetIOManager *assetIOManager = CoreAsset::AssetIOManager::GetInstance();

    RegisterAssetFactory();
    RegisterAssetLoader();
    RegisterAssetStorer();

    auto assetManager = CoreAsset::AssetManager::GetInstance();
    assetManager->Initialize(assetFactoryManager, assetIOManager, nullptr,
                             mGameRuntimeConfig->GetEngineAssetDirectory());
    assetManager->SetAssetRawDataPath(mGameRuntimeConfig->GetRawAssetDirectory());

    if (!LoadUserDLL())
        return;
    LoadAssets();
    //  InitRenderSystems();

    // asset load ( not raw data serialize)
    // LoadAssets();

    CreateWorkSpace();
    mWorld->SetEngineMode(mGameRuntimeMode.get());

    CoreAsset ::AssetPtr pFirstMap = assetManager->GetAsset<Map>(mGameBuildManifest.mStartupMapAssetID);
    Map *firstMap = pFirstMap.As<Map>();
    if (firstMap == nullptr)
    {
        return;
    }

    mWorld->Register(firstMap);
    mWorld->SetCurrentMap(firstMap);

    // map asset들 world에 모두 등록

    // 특정 map만 직렬화처리(일단 여기서 map 직렬화  )
    assetManager->LoadAssetRawData(firstMap);

    mInitialized = true;
}

void Quad::GameDirector::Begin()
{

    // world StartMap
    mWorld->StartMap();
}

void Quad::GameDirector::PreUpdate(float deltaTime)
{

    mGameWindowController->PreUpdate();
}

void Quad::GameDirector::Update(float deltaTime)
{
    mGameWindowController->Update(deltaTime);

    mWorld->Update(deltaTime);

    mRenderPipelineManager->Update(mApp->GetTotalFrameCount(), mApp->GetLastCompletedFenceValue());

    // world -> update(deltaTime);
}

void Quad::GameDirector::EndUpdate(float deltaTime)
{
    mWorld->EndUpdate(deltaTime);

    mGameWindowController->EndUpdate();
    // worlrd->endupdate(deltaTime);
}

void Quad::GameDirector::Draw()
{

    mGameWindowController->Draw();
}

void Quad::GameDirector::CleanUp()
{
    mWorld->CleanUp();
    // world->cleanUp();
}

void Quad::GameDirector::EndFrame() {}

void Quad::GameDirector::EndSystem()
{

    Render::RenderPipelineManager::GetInstance()->EndRenderThread();
    Render::AssetResolver::GetInstance()->EndResourceResolveThread();
}

Quad::GameRuntimeConfig *Quad::GameDirector::GetGameRuntimeConfig() const
{
    return mGameRuntimeConfig.get();
}

void Quad::GameDirector::CreateWorkSpace()
{
    mWorkSpace = std::make_unique<Core::WorkSpace>();

    CreateMainLogicalWindow();
    mWorkSpace->AddLogicalWindow(mMainLogicalWindow.get());

    mGameWindowController->SetWorkSpace(mWorkSpace.get());
}

void Quad::GameDirector::CreateMainLogicalWindow()
{

    mMainLogicalWindow = std::make_unique<Core::LogicalWindow>();

    mMainLogicalWindow->mViewportController.SetViewportMode(Core::EViewportMode::eAnchored);

    mMainLogicalWindow->mViewportController.SetAnchorLeftState(true);
    mMainLogicalWindow->mViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
    mMainLogicalWindow->mViewportController.SetAnchorLeftRelValue(0.0f);

    mMainLogicalWindow->mViewportController.SetAnchorRightState(true);
    mMainLogicalWindow->mViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
    mMainLogicalWindow->mViewportController.SetAnchorRightRelValue(0.0);

    mMainLogicalWindow->mViewportController.SetAnchorTopState(true);
    mMainLogicalWindow->mViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
    mMainLogicalWindow->mViewportController.SetAnchorTopRelValue(0.0f);
    // mMainSceneLogicalWindow->mViewportController.SetAnchorTopPixelValue(100.0f);

    mMainLogicalWindow->mViewportController.SetAnchorBottomState(true);
    mMainLogicalWindow->mViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mMainLogicalWindow->mViewportController.SetAnchorBottomRelValue(0.0f);

    mMainLogicalWindow->m3DWorldViewportController.SetViewportMode(Core::EViewportMode::eAnchored);
    mMainLogicalWindow->m3DWorldViewportController.SetAnchorLeftState(true);
    mMainLogicalWindow->m3DWorldViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
    mMainLogicalWindow->m3DWorldViewportController.SetAnchorLeftRelValue(0.0f);

    mMainLogicalWindow->m3DWorldViewportController.SetAnchorRightState(true);
    mMainLogicalWindow->m3DWorldViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
    mMainLogicalWindow->m3DWorldViewportController.SetAnchorRightRelValue(0.0f);

    mMainLogicalWindow->m3DWorldViewportController.SetAnchorTopState(true);
    mMainLogicalWindow->m3DWorldViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
    mMainLogicalWindow->m3DWorldViewportController.SetAnchorTopRelValue(0.0f);
    // mMainSceneLogicalWindow->mViewportController.SetAnchorTopPixelValue(0.0f);

    mMainLogicalWindow->m3DWorldViewportController.SetAnchorBottomState(true);
    mMainLogicalWindow->m3DWorldViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mMainLogicalWindow->m3DWorldViewportController.SetAnchorBottomRelValue(0.0f);

    mMainLogicalWindow->SetWorld(mWorld.get());
    mMainLogicalWindow->SetBackBufferClearColor(0.2f, 0.3f, 0.5f, 1.0f);

    // mDefaultEditWorkSpace->AddLogicalWindow(mMainSceneLogicalWindow.get());

    mMainLogicalWindow->SetDebugGridRender(false);

    /*   EditorSceneManager::GetInstance()->GetUserWorld()->mOnMapObjectRemovedCallbackSystem.Register(
           [](Object *object) { EditorSelectionManager::GetInstance()->OnMapObjectRemoved(object); });*/
}

void Quad::GameDirector::RegisterAssetFactory()
{
    CoreAsset::AssetFactoryManager *assetFactoryManager = CoreAsset::AssetFactoryManager::GetInstance();
    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::eTexture,
                                              CoreAsset::TextureFactory::GetInstance());
    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::eMaterial,
                                              CoreAsset::MaterialFactory::GetInstance());
    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::eStaticMesh,
                                              CoreAsset::MeshFactory::GetInstance());

    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::eSkinningMesh,
                                              CoreAsset::MeshFactory::GetInstance());

    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::eMap, Core::MapFactory::GetInstance());

    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::eFont, CoreAsset::FontFactory::GetInstance());

    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::ePrefab, PrefabFactory::GetInstance());
}

void Quad::GameDirector::RegisterAssetLoader()
{ // asset loader  register
    CoreAsset::AssetIOManager *assetIOManager = CoreAsset::AssetIOManager::GetInstance();

    assetIOManager->RegisterAssetLoader(CoreAsset::EAssetType::eTexture, CoreAsset::TextureLoader::GetInstance());
    assetIOManager->RegisterAssetLoader(CoreAsset::EAssetType::eMaterial, CoreAsset::MaterialLoader::GetInstance());
    assetIOManager->RegisterAssetLoader(CoreAsset::EAssetType::eStaticMesh, CoreAsset::MeshLoader::GetInstance());
    assetIOManager->RegisterAssetLoader(CoreAsset::EAssetType::eSkinningMesh, CoreAsset::MeshLoader::GetInstance());
    assetIOManager->RegisterAssetLoader(CoreAsset::EAssetType::eMap, Core::MapLoader::GetInstance());
    assetIOManager->RegisterAssetLoader(CoreAsset::EAssetType::ePrefab, PrefabLoader::GetInstance());
}

void Quad::GameDirector::RegisterAssetStorer()
{

    CoreAsset::AssetIOManager *assetIOManager = CoreAsset::AssetIOManager::GetInstance();

    // asset  storer register
    assetIOManager->RegisterAssetStorer(CoreAsset::EAssetType::eTexture, CoreAsset::TextureStorer::GetInstance());
    assetIOManager->RegisterAssetStorer(CoreAsset::EAssetType::eMaterial, CoreAsset::MaterialStorer::GetInstance());
    assetIOManager->RegisterAssetStorer(CoreAsset::EAssetType::eStaticMesh, CoreAsset::MeshStorer::GetInstance());
    assetIOManager->RegisterAssetStorer(CoreAsset::EAssetType::eSkinningMesh, CoreAsset::MeshStorer::GetInstance());
    assetIOManager->RegisterAssetStorer(CoreAsset::EAssetType::eMap, Core::MapStorer::GetInstance());
    assetIOManager->RegisterAssetStorer(CoreAsset::EAssetType::ePrefab, Core::PrefabStorer::GetInstance());
}

bool Quad::GameDirector::LoadUserDLL()
{

    const std::filesystem::path &path =
        mGameRuntimeConfig->GetGameRootDirectory() / mGameBuildManifest.mGameModuleRelativePath;

    mUserDLLHandle = CoreUtility::LibraryUtility::Load(path);

    if (mUserDLLHandle == nullptr)
        return false;

    return true;
}

bool Quad::GameDirector::LoadGameBuildManifest()
{

    Core::GameBuildManifest manifest;

    if (!ReadGameBuildManifest(mGameRuntimeConfig->GetManifestPath(), manifest))
        return false;

    mGameBuildManifest = std::move(manifest);

    return true;
}

void Quad::GameDirector::LoadAssets()
{

    // Gpu Sampler System. - >실제로 Editor에서 사용되는가?
    mGpuSamplerSystem = std::make_unique<GRM::GpuSamplerSystem>(GRM::IGpuResourceManager::GetInstance());

    // gpuSamplerSystem->LoadShaderSamplerFile()

    // shader buffer load
    //.shader.buffer 파일을 읽어서 gpuBuffer를 gpuBufferContextSystem에 등록한다.
    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();

    // gpuBufferContextSystem->LoadShaderBufferFile(mGameRuntimeConfig->GetGameRootDirectory() /
    //                                              "Shader/shaderbuffer.shader.buffer");

    auto gpuMaterialManager = Render::IMaterialManager::GetInstance();

    mUIMaterialManager = std::make_unique<CoreAsset::UIMaterialManager>(gpuMaterialManager);

    //  AssetResolver, ObjectRenderItemBuilder, UIRenderItemBuilder, IRenderProxyManager

    // Asset Resolver
    // Render::AssetResolver *assetResolver = Render::AssetResolver::GetInstance();

    //// GpuResourceManager는 플랫폼에 맞추어서 (이미 App 모듈에서 적절히 생성 - 초기화함 )
    // assetResolver->Initialize(CoreAsset::AssetManager::GetInstance(), GRM::IGpuResourceManager::GetInstance());

    auto assetManager = CoreAsset::AssetManager::GetInstance();

    auto pakSource = std::make_unique<CoreAsset::PakAssetDataSource>();

    if (!pakSource->Initialize(mGameRuntimeConfig->GetGameRootDirectory() / "Asset.pak"))
        return;

    std::vector<CoreAsset::AssetDataRecord> records;
    pakSource->GetAssetRecordList(records);

    assetManager->SetAssetDataSource(std::move(pakSource));
    CoreAsset::AssetLoadExecutionContext executionContext;

    // 1번 asset id (font-texture)
    auto it = std::find_if(records.begin(), records.end(),
                           [](const CoreAsset::AssetDataRecord &record)
                           {
                               if (record.mAssetID == 1)
                                   return true;

                               return false;
                           });
    // bootstrap asset
    if (it != records.end())
    {
        assetManager->LoadAsset(it->mAssetID, it->mRegistryPath, executionContext);
    }
    // builtin  asset

    Render::AssetResolver *assetResolver = Render::AssetResolver::GetInstance();

    // GpuResourceManager는 플랫폼에 맞추어서 (이미 App 모듈에서 적절히 생성 - 초기화함 )
    assetResolver->Initialize(CoreAsset::AssetManager::GetInstance(), GRM::IGpuResourceManager::GetInstance());

    // 나머지 pak 에셋

    for (const auto &record : records)
    {
        assetManager->LoadAsset(record.mAssetID, record.mRegistryPath, executionContext);
    }

    mObjectRenderItemBuilder = std::make_unique<Render::ObjectRenderItemBuilder>(
        Render::IRenderSystem::GetInstance(), GRM::IGpuResourceManager::GetInstance(),
        Render::AssetResolver::GetInstance());
    Core::IRenderProxyManager::SetRenderProxyManager(mObjectRenderItemBuilder.get());

    mUIRenderItemBuilder = std::make_unique<Render::UIRenderItemBuilder>(
        Render::IRenderSystem::GetInstance(), UI::UIManager::GetInstance(), GRM::IGpuResourceManager::GetInstance(),
        Render::AssetResolver::GetInstance());
}

void Quad::GameDirector::LoadBootstrapAssets() {}
