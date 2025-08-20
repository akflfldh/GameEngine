#pragma once

#include "CoreAsset/AssetPtr.h"
#include "CoreAsset/AssetType.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
#include <RenderSystem/MaterialType.h>
#include <memory>
#include <unordered_map>

namespace Render
{
class IMaterialManager;
}

namespace CoreAsset
{

constexpr Render::MaterialID DefaultUIGpuMaterialID = 3;
constexpr Render::MaterialID DefaultGizmoUIGpuMaterialID = 4;

class Texture;
class Material;
// Asset머터리얼 매니저의 윗 계층에 존재하는 UIMaterial매니저
// 래핑클래스같은 역할
class CORE_ASSET_API UIMaterialManager
{
  public:
    static UIMaterialManager *GetInstance();
    UIMaterialManager(Render::IMaterialManager *gpuMaterialManager);
    ~UIMaterialManager();

    UIMaterialManager(const UIMaterialManager &) = delete;
    UIMaterialManager &operator=(const UIMaterialManager &) = delete;

    CoreAsset::Material *GetOrCreateDefaultMaterial(CoreAsset::Texture *texture = nullptr);
    CoreAsset::Material *GetOrCreateDefaultMaterial(CoreAsset::AssetID textureID = 0);

    // 사이즈조절을 위한 테투리용 머터리얼
    CoreAsset::Material *GetDefaultGizmoMaterial();

  private:
    CoreAsset::Material *GetDefaultMaterial(CoreAsset::Texture *texture) const;
    CoreAsset::Material *CreateDefaultMaterial(CoreAsset::Texture *texture);

    CoreAsset::Material *CreateDefaultGizmoMaterial();

  private:
    static UIMaterialManager *mInstance;
    Render::IMaterialManager *mGpuMaterialManager;

    std::unordered_map<AssetID, std::unique_ptr<CoreAsset::Material>> mDefaultMaterialTable;

    std::unique_ptr<CoreAsset::Material> mDefaultGizmoMaterial;
};

} // namespace CoreAsset
