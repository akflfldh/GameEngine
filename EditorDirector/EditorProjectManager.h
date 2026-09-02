#pragma once

#include <CoreBase/CallbackSystem.h>
#include <filesystem>
#include <memory>
#include <string>
class Map;

using OnOpenMapCallbackSystem = Core::MultiCallbackSystem<Map *>;

namespace QuadPF
{

class PhysicalFileSystem;
}

namespace QuadLF
{
class LogicalFileSystem;
}

namespace CoreAsset
{
class AssetLoader;
class AssetMetaDataManager;
class UIMaterialManager;
} // namespace CoreAsset

namespace GRM
{
class GpuSamplerSystem;
}
namespace Render
{
class UIRenderItemBuilder;
class ObjectRenderItemBuilder;
} // namespace Render

namespace CoreAsset
{
class Asset;
}

class EditorMode;
class EditorEditMode;
class BaseSelectionManager;

namespace Quad
{

// 프로젝트에 관한 부분에대해서 초기셋팅을 수행한다.

class SerializedAssetTContainer;
class EditorTextureImporter;
class EditorShaderImporter;

class EditorProjectManager
{
  public:
    static EditorProjectManager *GetInstance();

    EditorProjectManager();
    ~EditorProjectManager();

    bool Initialize();
    bool InitProject();

    void InitEditorMap();

    void SaveProject();

    void OpenMap(Map *map);
    void SaveMap(Map *map);

    OnOpenMapCallbackSystem mOnOpendMapCallbackSystem;
    void SetDrawDebugColliderFlag(bool flag);
    bool GetDrawDebugColliderFlag() const;

    void SaveAsset(CoreAsset::Asset *asset);

    // map에 에디터용오브젝트들을 생성한다
    void CreateEditorObjects(Map *map, BaseSelectionManager *selectionManager);

  private:
    void InitSystems();
    void SaveAsset();
    void SaveMap();
    void SaveLogicalDirectoryStructure();

    void LoadProject();

    bool InternalSaveSingleAsset(CoreAsset::Asset *asset);

    void LoadUserProjectDLL();

  private:
    // 논리적파일시스템을 프로젝트의 디렉터리구조로 초기화한다.
    void InitLogicalDirectoryStructure();

    // 프로젝트의 asset들을 로드한다.
    // 디렉터리 계층구조내에 존재하는 에셋들을 모두 로드한다.
    void LoadProjectAsset();

    // Editor에서만 사용하는 Asset 생성
    void CreateEditorAsset();
    // Editor/Asset의 Asset들을 로드
    void LoadEditorAsset();

    void LoadProjectCXXList();

    // 로드한 asset들의 인스턴스와 해당 logicalFile들을 생성한다
    // void CreateAssetAndLogicalFile();
    // void CreateTextureAndLogicalFile(SerializedAssetTContainer *serializedAssetContainer);
    // void CreateMaterialAndLogicalFile(SerializedAssetTContainer *serializedAssetContainer);

// core로 이전될가능성있는것들
#pragma region ToCore

    void RegisterCallbackShader();

#pragma endregion

    void CreateDefaultUserMap();

  private:
    static EditorProjectManager *mInstance;

    // QuadPF::PhysicalFileSystem *mPhysicalFileSystem;
    // CoreAsset::AssetLoader *mAssetLoader;

    // std::unique_ptr<QuadLF::LogicalFileSystem> mLogicalFileSystem;

    // std::unique_ptr<EditorTextureImporter> mTextureImporter;
    // std::unique_ptr<GRM::GpuSamplerSystem> mGpuSamplerSystem;
    // std::unique_ptr<EditorShaderImporter> mEditorShaderImporter;
    // std::unique_ptr<Render::UIRenderItemBuilder> mUIRenderItemBuilder;
    // std::unique_ptr<CoreAsset::UIMaterialManager> mUIMaterialManager;
    // CoreAsset::AssetMetaDataManager *mAssetMetaDataManager;
    // std::unique_ptr<Render::ObjectRenderItemBuilder> mObjectRenderItemBuilder;
    //  projectpath / RawAsset 으로 고정
    std::filesystem::path mAssetRawFolderPath;

    std::unique_ptr<EditorEditMode> mEditorMode;
    void *mUserProjectDLLHandle;
};

} // namespace Quad
