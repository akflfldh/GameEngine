#include "AssetResolver.h"
#include <CoreAsset/Asset.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/Texture.h>
#include <D3DGpuResourceManager/IGpuResource.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>

Render::AssetResolver *Render::AssetResolver::GetInstance()
{
    static AssetResolver instance;

    return &instance;
}

Render::AssetResolver::AssetResolver() {}

Render::AssetResolver::~AssetResolver() {}

void Render::AssetResolver::Initialize(CoreAsset::AssetManager *assetManager,
                                       GRM::IGpuResourceManager *gpuResourceManager)
{
    mAssetManager = assetManager;
    mGpuResourceManager = gpuResourceManager;
}

bool Render::AssetResolver::ResolveAsset(CoreAsset::Asset *asset)
{

    if (asset == nullptr)
        return true;

    switch (asset->GetType())
    {
    case CoreAsset::EAssetType::eTexture:
        return ResolveTexture(static_cast<CoreAsset::Texture *>(asset));

        break;

    case CoreAsset::EAssetType::eMaterial:

        return ResolveMaterial(static_cast<CoreAsset::Material *>(asset));

        break;
    }
    // 향후 다른타입에셋들도 추가

    return true;
}

GRM::GRMPtr Render::AssetResolver::GetGpuResource(CoreAsset::Asset *asset) const
{
    if (asset == nullptr)
        return nullptr;

    CoreAsset::AssetID assetID = asset->GetID();

    return GetGpuResource(assetID);
}

GRM::GRMPtr Render::AssetResolver::GetGpuResource(CoreAsset::AssetID assetID) const
{

    if (assetID == NoneAssetID)
        return nullptr;

    std::unordered_map<CoreAsset::AssetID, GRM::GRMPtr>::const_iterator it = mAssetGpuResourceTable.find(assetID);

    if (it == mAssetGpuResourceTable.cend())
        return nullptr;

    return it->second;
}

bool Render::AssetResolver::ResolveMaterial(CoreAsset::Material *material)
{

    // material 에서 참조하는 texture resource들을대해서
    // 재귀적으로 ResolveTexture를 호출한다.
    //  material자체는 메타데이터와 raw데이터가 같이 로드되기에따로 raw데이터처리는없다.
    return true;
}

bool Render::AssetResolver::ResolveTexture(CoreAsset::Texture *texture)
{

    if (texture == nullptr)
        return true;

    CoreAsset::AssetID assetID = texture->GetID();

    GRM::GRMPtr grmPtr = GetGpuResource(assetID);
    // 먼저 texture의 raw데이터가 로드되어서 gpuResource가 존재하는지
    if (grmPtr.getResource() != nullptr)
    {
        // 이미 존재
        return true;
    }
    // 없다면 assetManagaer에게 rawData 로드 요청

    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();
    assetManager->LoadAssetRawData(texture);

    GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    GRM::TextureDesc textureDesc;
    textureDesc = texture->GetProperties().mMetaData;

    GRM::GRMPtr gpuResource = gpuResourceManager->CreateTexture(textureDesc);

    if (gpuResource.getResource() == nullptr)
    {
        // log
        // 실패했다는것을 알려야하고, 상위시스템은 렌더링을 막아야할것이다.
        return false;
    }

    // 그후 gpuResourceManager에게 적절한 데이터를 넘겨서 gpuResourcec생성
    RegisterGpuReosurce(texture->GetID(), gpuResource);
    // 그렇게 생성된 gpuResource를 texture 에셋 멤버변수에 대입

    // empty flag = true 처리 //보류

    // 리턴
    return true;
}

void Render::AssetResolver::RegisterGpuReosurce(CoreAsset::AssetID assetID, GRM::GRMPtr ptr)
{

    mAssetGpuResourceTable[assetID] = ptr;
}
