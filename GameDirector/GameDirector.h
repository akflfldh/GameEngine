#pragma once

#include <Core/GameBuildManifest.h>
#include <Core/IProgramDirector.h>
#include <filesystem>
#include <memory>

namespace Render
{
class RenderPipelineManager;
class ObjectRenderItemBuilder;
class UIRenderItemBuilder;
} // namespace Render

class World;

namespace Core
{
class WorkSpace;

class LogicalWindow;

} // namespace Core
namespace CoreAsset
{
class UIMaterialManager;
}

namespace GRM
{
class GpuSamplerSystem;
}

namespace Quad
{
class GameWindow;
class GameWindowController;
class MapMetaData;
class Project;
class Application;
class GameRuntimeMode;
class GameRuntimeConfig;

class GameDirector : public Quad::IProgramDirector
{
  public:
    static GameDirector *GetInstance();
    GameDirector();
    virtual ~GameDirector();

    // Initialize 호출전에 호출하자
    void InitRuntimeConfig(const std::filesystem::path &gameRootPath);

    virtual void Initialize() override;

    virtual void Begin() override;
    virtual void PreUpdate(float deltaTime) override;
    virtual void Update(float deltaTime) override;
    virtual void EndUpdate(float deltaTime) override;
    virtual void Draw() override;
    virtual void CleanUp() override;
    virtual void EndFrame() override;
    virtual void EndSystem() override;

    GameRuntimeConfig *GetGameRuntimeConfig() const;

  private:
    void CreateWorkSpace();
    void CreateMainLogicalWindow();

    void RegisterAssetFactory();
    void RegisterAssetLoader();
    void RegisterAssetStorer();

    bool LoadUserDLL();
    bool LoadGameBuildManifest();
    void LoadAssets();

    /*
    렌더와 관련된 시스템들 초기화
    */

    void LoadBootstrapAssets();

  private:
    std::unique_ptr<GameWindowController> mGameWindowController;
    std::unique_ptr<Core::WorkSpace> mWorkSpace;
    std::unique_ptr<Core::LogicalWindow> mMainLogicalWindow;
    std::unique_ptr<World> mWorld;
    std::unique_ptr<GameRuntimeConfig> mGameRuntimeConfig;

    Core::GameBuildManifest mGameBuildManifest;

    Application *mApp = nullptr;
    std::unique_ptr<GameRuntimeMode> mGameRuntimeMode;
    Render::RenderPipelineManager *mRenderPipelineManager = nullptr;
    std::unique_ptr<CoreAsset::UIMaterialManager> mUIMaterialManager;
    std::unique_ptr<Render::ObjectRenderItemBuilder> mObjectRenderItemBuilder;
    std::unique_ptr<GRM::GpuSamplerSystem> mGpuSamplerSystem;
    std::unique_ptr<Render::UIRenderItemBuilder> mUIRenderItemBuilder;

    void *mUserDLLHandle = nullptr;
};

} // namespace Quad