#pragma once

#include "CoreAsset/AssetPtr.h"
#include "CoreAsset/AssetType.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
#include <memory>
#include <stack>
#include <unordered_map>

namespace CoreAsset
{

class Texture;
class Material;
// Asset머터리얼 매니저의 윗 계층에 존재하는 UIMaterial매니저
// 래핑클래스같은 역할
class CORE_ASSET_API UIMaterialManager
{
  public:
    static UIMaterialManager *GetInstance();
    UIMaterialManager();
    ~UIMaterialManager();

    UIMaterialManager(const UIMaterialManager &) = delete;
    UIMaterialManager &operator=(const UIMaterialManager &) = delete;

    CoreAsset::Material *GetOrCreateDefaultMaterial(CoreAsset::Texture *texture = nullptr);
    CoreAsset::Material *GetOrCreateDefaultMaterial(CoreAsset::AssetID textureID = 0);

    // 사이즈조절을 위한 테투리용 머터리얼
    CoreAsset::Material *GetDefaultGizmoMaterial();

    CoreAsset::Material *GetOrCreateDefaultFontMaterial(CoreAsset::Texture *texture = nullptr);
    CoreAsset::Material *GetOrCreateDefaultFontMaterial(CoreAsset::AssetID textureID = 0);

  private:
    CoreAsset::Material *GetDefaultMaterial(CoreAsset::Texture *texture) const;
    CoreAsset::Material *CreateDefaultMaterial(CoreAsset::Texture *texture);

    CoreAsset::Material *CreateDefaultGizmoMaterial();

    void CreateDefaultFontMaterial(CoreAsset::Texture *texture);

    CoreAsset::AssetID GetNextAssetID();

  private:
    static UIMaterialManager *mInstance;

    std::unordered_map<AssetID, std::unique_ptr<CoreAsset::Material>> mDefaultMaterialTable;

    std::unique_ptr<CoreAsset::Material> mDefaultGizmoMaterial;
    std::unique_ptr<CoreAsset::Material> mDefaultFontMaterial;

    std::stack<AssetID> mAssetIDFreeList;
    CoreAsset::AssetID mNextAssetID = 1;
};

} // namespace CoreAsset
