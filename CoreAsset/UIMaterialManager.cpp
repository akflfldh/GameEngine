#include "CoreAsset/UIMaterialManager.h"
#include "CoreAsset/Material.h"
#include <CoreAsset/AssetManager.h>
#include <Logger/Logger.h>
#include <RenderSystem/IMaterialManager.h>
#include <assert.h>

CoreAsset::UIMaterialManager *CoreAsset::UIMaterialManager::mInstance = nullptr;

CoreAsset::UIMaterialManager *CoreAsset::UIMaterialManager::GetInstance()
{
    if (mInstance == nullptr)
    {
        LOG_MESSAGE_CRITICAL("UIMaterialManager", "UI머터리얼매니저인스턴스를 생성해야합니다.");
        assert(0);
    }

    return mInstance;
}

CoreAsset::UIMaterialManager::UIMaterialManager(Render::IMaterialManager *gpuMaterialManager)
    : mGpuMaterialManager(gpuMaterialManager), mDefaultGizmoMaterial(nullptr), mNextAssetID(1)
{
    if (mInstance != nullptr)
    {
        LOG_MESSAGE_CRITICAL("UIMaterialManager", "UI머터리얼매니저은 한번만 생성해야한다.");
        assert(0);
    }

    mInstance = this;
}

CoreAsset::UIMaterialManager::~UIMaterialManager() {}

CoreAsset::Material *CoreAsset::UIMaterialManager::GetOrCreateDefaultMaterial(CoreAsset::Texture *texture)
{
    if (texture == nullptr)
    {
        // 디폴트 텍스처를 사용하자

        AssetManager *assetManager = AssetManager::GetInstance();
        texture = static_cast<Texture *>(assetManager->GetDefaultAsset(EAssetType::eTexture).Get());
    }

    Material *material = GetDefaultMaterial(texture);

    if (material == nullptr)
        material = CreateDefaultMaterial(texture, mDefaultUIGpuMaterialID);

    return material;
}

CoreAsset::Material *CoreAsset::UIMaterialManager::GetOrCreateDefaultMaterial(CoreAsset::AssetID textureID)
{

    AssetManager *assetManager = AssetManager::GetInstance();
    Texture *texture = static_cast<Texture *>(assetManager->GetAsset<Texture>(textureID).Get());

    return GetOrCreateDefaultMaterial(texture);
}

CoreAsset::Material *CoreAsset::UIMaterialManager::GetOrCreateDefaultFontMaterial(CoreAsset::Texture *texture)
{

    if (mDefaultFontMaterial == nullptr)
        CreateDefaultFontMaterial(texture);

    return mDefaultFontMaterial.get();
}

CoreAsset::Material *CoreAsset::UIMaterialManager::GetOrCreateDefaultFontMaterial(CoreAsset::AssetID textureID)
{

    AssetManager *assetManager = AssetManager::GetInstance();
    Texture *texture = static_cast<Texture *>(assetManager->GetAsset<Texture>(textureID).Get());

    return GetOrCreateDefaultFontMaterial(texture);
}

void CoreAsset::UIMaterialManager::RegisterDefaultUIGpuMaterialID(Render::MaterialID id)
{

    mDefaultUIGpuMaterialID = id;
}
void CoreAsset::UIMaterialManager::RegsiterDefaultUIFontGpuMaterialID(Render::MaterialID id)
{

    mDefaultFontGpuMaterialID = id;
}

CoreAsset::Material *CoreAsset::UIMaterialManager::GetDefaultGizmoMaterial()
{
    if (mDefaultGizmoMaterial == nullptr)
        CreateDefaultGizmoMaterial();

    return mDefaultGizmoMaterial.get();
}

CoreAsset::Material *CoreAsset::UIMaterialManager::GetDefaultMaterial(CoreAsset::Texture *texture) const
{

    std::unordered_map<AssetID, std::unique_ptr<CoreAsset::Material>>::const_iterator it =
        mDefaultMaterialTable.find(texture->GetID());
    if (it == mDefaultMaterialTable.cend())
        return nullptr;

    return it->second.get();
}

CoreAsset::Material *CoreAsset::UIMaterialManager::CreateDefaultMaterial(CoreAsset::Texture *texture,
                                                                         Render::MaterialID id)
{
    // TODO material asset id
    std::unique_ptr<Material> newMaterial(new Material(GetNextAssetID()));
    newMaterial->mGpuMaterialID = id;
    newMaterial->SetUseExplicitGpuMaterial(true);

    // 기본 ui머터리얼은 리소스가 텍스처 하나일뿐이다. 이코드는 더 간단히 수정가능
    // 샘플러도 하나

    newMaterial->AddAlbedoTexSlot();
    newMaterial->SetTextureResource(0, texture);

    mDefaultMaterialTable[texture->GetID()] = std::move(newMaterial);

    return mDefaultMaterialTable[texture->GetID()].get();
}

CoreAsset::Material *CoreAsset::UIMaterialManager::CreateDefaultGizmoMaterial()
{

    std::unique_ptr<Material> newMaterial(new Material(GetNextAssetID()));
    // newMaterial->mGpuMaterialID = DefaultGizmoUIGpuMaterialID;

    // const Render::ShaderResourceInfoSet &shaderResourceInfoSet =
    //     mGpuMaterialManager->GetMaterialShaderResourceInfo(newMaterial->mGpuMaterialID);

    // newMaterial->mTexResourceContextList.resize(shaderResourceInfoSet.mObjectTextureShaderResourceInfoVector.size());
    // newMaterial->mSamplerResourceContextList.resize(
    //     shaderResourceInfoSet.mObjectSamplerShaderResourceInfoVector.size());

    // mDefaultGizmoMaterial = std::move(newMaterial);

    return mDefaultGizmoMaterial.get();
}

void CoreAsset::UIMaterialManager::CreateDefaultFontMaterial(CoreAsset::Texture *texture)
{

    std::unique_ptr<Material> newMaterial(new Material(GetNextAssetID()));
    newMaterial->mGpuMaterialID = mDefaultFontGpuMaterialID;

    newMaterial->AddAlbedoTexSlot();

    // 기본 ui머터리얼은 리소스가 텍스처 하나일뿐이다. 이코드는 더 간단히 수정가능
    // 샘플러도 하나

    newMaterial->SetTextureResource(0, texture);

    mDefaultFontMaterial = std::move(newMaterial);
}

CoreAsset::AssetID CoreAsset::UIMaterialManager::GetNextAssetID()
{
    if (mAssetIDFreeList.empty() == false)
    {
        CoreAsset::AssetID id = mAssetIDFreeList.top();
        mAssetIDFreeList.pop();
        return id;
    }

    return mNextAssetID++;
}