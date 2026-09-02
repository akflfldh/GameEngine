#pragma once

#include <LogicalFileSystem/LogicalCommon.h>
#include <RenderSystem/MaterialType.h>
#include <Windows.h>
#include <d3d12.h>
#include <string>

namespace CoreAsset
{
class TextureManager;
class MaterialManager;
} // namespace CoreAsset
namespace QuadLF
{
class LogicalFileSystem;
class LogicalFolder;

} // namespace QuadLF

namespace Quad
{

class EditorAssetManager;

struct ImGpuAssetMaterialCreationContext
{
    Render::MaterialID mGpuMaterialID;
    std::vector<std::pair<std::string, CoreAsset::AssetID>> mTexResourceInfo;
    std::vector<uint32_t> mSamplerResourceInfo;

    char mAssetMaterialName[255];
};

class ImGuiSystem
{
  public:
    static ImGuiSystem *GetInstance();
    ImGuiSystem();
    ~ImGuiSystem();

    void Initialize();

    void InitPlatform(HWND hwnd, ID3D12Device *device, ID3D12CommandQueue *commandQueue);

    void SetDependency(QuadLF::LogicalFileSystem *logicalFileSystem, CoreAsset::TextureManager *textureManager,
                       CoreAsset::MaterialManager *materialManager, EditorAssetManager *editorAssetManager);

    void Draw();

  private:
    void DrawEditorBuildInfo();
    void DrawLogicalFileSystem();
    void DrawLogicalFolder(QuadLF::LogicalFolder *logicalFolder);
    void DrawSelectAsset();
    void DrawCreationMaterialPanel();
    void DrawSavePanel();

  private:
    ID3D12DescriptorHeap *m_ImGuiSrvHeap;
    ID3D12CommandQueue *mCommandQueue;

    ID3D12CommandAllocator *mCommandAllcator;
    ID3D12GraphicsCommandList *mCommandList;

    HWND mHwnd;
    QuadLF::LogicalFileSystem *mLogicalFileSystem;
    CoreAsset::TextureManager *mTextureManager;
    CoreAsset::MaterialManager *mMaterialManager;
    EditorAssetManager *mEditorAssetManager;

    bool selectedA = false;
    bool selectedB = true;
    bool AssetClick = false;
    QuadLF::LogicalFileAssetInfo assetInfo;

    ImGpuAssetMaterialCreationContext mCurrMaterialCreationContext;
    bool mSetGpuMaterialID = false;
    bool bMaterialCreationresult = true;
};

} // namespace Quad
