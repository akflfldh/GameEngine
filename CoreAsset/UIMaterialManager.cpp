#include "CoreAsset/UIMaterialManager.h"
#include "CoreAsset/Material.h"
#include "CoreAsset/TextureManager.h"
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
    : mGpuMaterialManager(gpuMaterialManager), mDefaultGizmoMaterial(nullptr)
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

        TextureManager *textureManager = TextureManager::GetInstance();
        texture = textureManager->GetAsset(DefaultTextureID);
    }

    Material *material = GetDefaultMaterial(texture);

    if (material == nullptr)
        material = CreateDefaultMaterial(texture);

    return material;
}

CoreAsset::Material *CoreAsset::UIMaterialManager::GetOrCreateDefaultMaterial(CoreAsset::AssetID textureID)
{

    TextureManager *textureManager = TextureManager::GetInstance();
    Texture *texture = textureManager->GetAsset(textureID);

    return GetOrCreateDefaultMaterial(texture);
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

CoreAsset::Material *CoreAsset::UIMaterialManager::CreateDefaultMaterial(CoreAsset::Texture *texture)
{

    std::unique_ptr<Material> newMaterial(new Material(0, ""));
    newMaterial->mGpuMaterialID = DefaultUIGpuMaterialID;

    const Render::ShaderResourceInfoSet &shaderResourceInfoSet =
        mGpuMaterialManager->GetMaterialShaderResourceInfo(newMaterial->mGpuMaterialID);

    newMaterial->mTexResourceContextList.resize(shaderResourceInfoSet.mTextureShaderResourceInfoVector.size());
    newMaterial->mSamplerResourceContextList.resize(shaderResourceInfoSet.mSamplerShaderResourceInfoVector.size());

    // 기본 ui머터리얼은 리소스가 텍스처 하나일뿐이다. 이코드는 더 간단히 수정가능
    // 샘플러도 하나
    for (int i = 0; i < newMaterial->mTexResourceContextList.size(); ++i)
    {
        newMaterial->mTexResourceContextList[i].mTexShaderResourceInfo =
            shaderResourceInfoSet.mTextureShaderResourceInfoVector[i];
    }
    newMaterial->mTexResourceContextList[0].mTexture = texture;
    newMaterial->mSamplerResourceContextList[0] = shaderResourceInfoSet.mSamplerShaderResourceInfoVector[0].mSamplerID;

    mDefaultMaterialTable[texture->GetID()] = std::move(newMaterial);

    return mDefaultMaterialTable[texture->GetID()].get();
}

CoreAsset::Material *CoreAsset::UIMaterialManager::CreateDefaultGizmoMaterial()
{

    std::unique_ptr<Material> newMaterial(new Material(1, ""));
    newMaterial->mGpuMaterialID = DefaultGizmoUIGpuMaterialID;

    const Render::ShaderResourceInfoSet &shaderResourceInfoSet =
        mGpuMaterialManager->GetMaterialShaderResourceInfo(newMaterial->mGpuMaterialID);

    newMaterial->mTexResourceContextList.resize(shaderResourceInfoSet.mTextureShaderResourceInfoVector.size());
    newMaterial->mSamplerResourceContextList.resize(shaderResourceInfoSet.mSamplerShaderResourceInfoVector.size());

    mDefaultGizmoMaterial = std::move(newMaterial);

    return mDefaultGizmoMaterial.get();
}