#pragma once
// #include"header.h"
#include "Predeclare.h"
#include <Core/IProgramDirector.h>
// #include"Core/GraphicCommand.h"
// #include"Core/RenderSystem.h"

// #include"EditorDirector/ProjectDirector.h"
// #include"EditorDirector/FrameWindowMenuDirector.h"
// #include"EditorDirector/EditorModeDirector.h"

// #include"BuildModule.h"
#include <memory>
#include <string>
#include <vector>
#define EngineMode

namespace GRM
{
class GpuSamplerSystem;
class IGpuResourceManager;
} // namespace GRM

namespace SystemInitializer
{
class ISystemInitializer;
}
namespace QuadLF
{
class LogicalFileSystem;
}
namespace QuadPF
{
class PhysicalFileSystem;
}

namespace CoreAsset
{
class AssetLoader;
class UIMaterialManager;
class AssetMetaDataManager;
class AssetManager;
class Material;
} // namespace CoreAsset

namespace Render
{
class ObjectRenderItemBuilder;
class RenderPipelineManager;
class UIRenderItemBuilder;
class IRenderSystem;
} // namespace Render

namespace Core
{
class SuperController;
class LogicalWindow;
class WorkSpace;

} // namespace Core

namespace UI
{
class UIManager;
}

namespace Quad
{
//	class FileUiSystem;
// class ImportModule;

class SuperFrameController;
class SuperAssetBrowerController;
class EditorTextureImporter;
class EditorProjectManager;
class EditorShaderImporter;
class Application;
class IEditorTaskManager;

class EditorDirector : public IProgramDirector
{
  public:
    static EditorDirector *GetInstance();
    EditorDirector();
    virtual ~EditorDirector();

    virtual void Initialize() override;
    virtual void Initialize(const RuntimeServices &services) override;

    virtual void Begin() override;
    virtual void PreUpdate(float deltaTime) override;
    virtual void Update(float deltaTime) override;
    virtual void EndUpdate(float deltaTime) override;
    virtual void CleanUp() override;

    virtual void Draw() override;

    virtual void EndFrame() override;

    void ShutDownWindow();

    virtual void EndSystem() override;

    void SetPlayModeState(bool state);
    static bool GetPlayModeState();

    // 프레임윈도우씬으로 화면전환
    // 다른 윈도우들은 정지 그리고 사라진다.

    // 매프레임첫시작에 상태를보고 전환한다.
    static void SetSwitchWindowSceneModeFlag(int flag);

    const int GetSwitchWindowSceneModeFlag() const;

    Core::LogicalWindow *GetMainSceneWindow() const;

    void ChangeToPrefabEditWorkSpace();
    void ChangeToMaterialEditWorkSpace(CoreAsset::Material *targetMaterial);
    void ChangeToDefaultEditWorkSpace();

    void ChangeWorkSpace(Core::WorkSpace *workspace);

  private:
    SuperFrameController *mSuperFrameController;
    SuperAssetBrowerController *mSuperAssetBrowerController;

    std::vector<Core::SuperController *> mSuperControllerVector;

  private:
    void LoadEditorAssets();

    // Sub Init Method
    // 시스템들을 초기화한다.
    //   void InitSystem();

    // 프로젝트 선택 단계만을 위한 윈도우생성
    void InitProjectBrowserWindow();

    void InitEngineAssetLogicalFile();

    void CreateEditWorkSpace();

    void CreateDefaultEditWorkSpace();
    void CreatePrefabEditWorkSpace();
    void CreateMaterialEditWorkSpace();

    void InitEditorWindows();
    void InitMainSceneWindow();
    void InitAssetBrowerWindow();
    void InitGlobalOverlayWindow();
    void InitPropertyWindow();

    void InitEditorTaskManagerList();

    Core::LogicalWindow *GetMainWindow() const;

    void RegisterAssetFactory();
    void RegisterAssetLoader();
    void RegisterAssetStorer();

    // switchFrameWindowSceneFlag를보고 프레임윈도우(단독)으로 전환한다.
    void SwitchFrameWindow();
    void SwitchCommonEditWindow();

    void InitSystems();

    void UpdateEditorTaskManagers();

    Quad::Application *mApp;

    bool mIspaused = false;
    bool mIsMinimized = false;
    bool mIsMaximized = false;
    bool mIsResizing = false;

  private:
    EditorProjectManager *mProjectInitializer;

    const RuntimeServices *mRuntimeServices = nullptr;
    CoreAsset::AssetManager *mAssetManager = nullptr;
    UI::UIManager *mUIManager = nullptr;
    Render::IRenderSystem *mRenderSystem = nullptr;
    GRM::IGpuResourceManager *mGpuResourceManager = nullptr;

    Render::RenderPipelineManager *mRenderPipelineManager;

    std::unique_ptr<Core::WorkSpace> mDefaultEditWorkSpace = nullptr;
    std::unique_ptr<Core::WorkSpace> mPrefabEditWorkSpace = nullptr;
    std::unique_ptr<Core::WorkSpace> mProjectBrowserWorkSpace = nullptr;

    std::unique_ptr<Core::LogicalWindow> mMainSceneLogicalWindow = nullptr;
    std::unique_ptr<Core::LogicalWindow> mPropertyLogicalWindow = nullptr;
    std::unique_ptr<Core::LogicalWindow> mAssetBrowserLogicalWindow = nullptr;
    std::unique_ptr<Core::LogicalWindow> mGlobalOverlayLogicalWindow = nullptr;
    std::unique_ptr<Core::LogicalWindow> mProjectBrowserLogicalWindow = nullptr;

    QuadPF::PhysicalFileSystem *mPhysicalFileSystem;
    CoreAsset::AssetLoader *mAssetLoader;

    std::unique_ptr<QuadLF::LogicalFileSystem> mLogicalFileSystem;

    std::unique_ptr<EditorTextureImporter> mTextureImporter;
    std::unique_ptr<GRM::GpuSamplerSystem> mGpuSamplerSystem;
    std::unique_ptr<EditorShaderImporter> mEditorShaderImporter;
    std::unique_ptr<Render::UIRenderItemBuilder> mUIRenderItemBuilder;
    std::unique_ptr<CoreAsset::UIMaterialManager> mUIMaterialManager;
    CoreAsset::AssetMetaDataManager *mAssetMetaDataManager;
    std::unique_ptr<Render::ObjectRenderItemBuilder> mObjectRenderItemBuilder;

    std::vector<IEditorTaskManager *> mEditorTaskManagerList;
};

} // namespace Quad