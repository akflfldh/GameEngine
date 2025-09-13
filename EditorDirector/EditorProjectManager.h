#pragma once

#include <memory>
#include <string>
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
}

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

    void SaveProject();

  private:
    void SaveAsset();
    void SaveMap();

  private:
    // 논리적파일시스템을 프로젝트의 디렉터리구조로 초기화한다.
    void InitLogicalDirectoryStructure();

    // 프로젝트의 asset들을 로드한다.
    // 디렉터리 계층구조내에 존재하는 에셋들을 모두 로드한다.
    void LoadProjectAsset();

    // 로드한 asset들의 인스턴스와 해당 logicalFile들을 생성한다
    void CreateAssetAndLogicalFile();
    void CreateTextureAndLogicalFile(SerializedAssetTContainer *serializedAssetContainer);
    void CreateMaterialAndLogicalFile(SerializedAssetTContainer *serializedAssetContainer);

  private:
    static EditorProjectManager *mInstance;

    QuadPF::PhysicalFileSystem *mPhysicalFileSystem;
    CoreAsset::AssetLoader *mAssetLoader;

    std::unique_ptr<QuadLF::LogicalFileSystem> mLogicalFileSystem;

    std::unique_ptr<EditorTextureImporter> mTextureImporter;
    std::unique_ptr<GRM::GpuSamplerSystem> mGpuSamplerSystem;
    std::unique_ptr<EditorShaderImporter> mEditorShaderImporter;
    std::unique_ptr<Render::UIRenderItemBuilder> mUIRenderItemBuilder;
    std::unique_ptr<CoreAsset::UIMaterialManager> mUIMaterialManager;
    CoreAsset::AssetMetaDataManager *mAssetMetaDataManager;

    // projectpath / RawAsset 으로 고정
    std::string mAssetRawFolderPath;
};

} // namespace Quad
