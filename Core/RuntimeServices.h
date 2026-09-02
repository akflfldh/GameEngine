#pragma once

namespace CoreAsset
{
class AssetManager;
}

namespace UI
{
class UIManager;
}

namespace Render
{
class IRenderSystem;
}

namespace GRM
{
class IGpuResourceManager;
}

struct RuntimeServices
{
    CoreAsset::AssetManager *mAssetManager = nullptr;
    UI::UIManager *mUIManager = nullptr;
    Render::IRenderSystem *mRenderSystem = nullptr;
    GRM::IGpuResourceManager *mGpuResourceManager = nullptr;
};
