#include "CoreAsset/TextureManager.h"
#include "CoreAsset/AssetMetaDataManager.h"
#include "CoreAsset/GlobalAssetRegistrySystem.h"
#include "CoreAsset/Texture.h"
#include <Utility/Utility.h>
#include <assert.h>

CoreAsset::TextureManager *CoreAsset::TextureManager::mInstance = nullptr;

CoreAsset::TextureManager *CoreAsset::TextureManager::GetInstance()
{
    if (mInstance == nullptr)
    {
        // log
        assert(0);
    }

    return mInstance;
}

CoreAsset::TextureManager::TextureManager()
{

    if (mInstance != nullptr)
    {
        // log
        assert(0);
    }
    mInstance = this;
    mGlobalAssetRegistrySystem = GlobalAssetRegistrySystem::GetInstance();
}
CoreAsset::TextureManager::~TextureManager() {}

CoreAsset::Texture *CoreAsset::TextureManager::CreateTexture(const GRM::TextureDesc &textureDesc,
                                                             const std::string &name, const std::string &path,
                                                             bool keepRawDataFlag) // 동적으로 생성
{

    //// gpuResourrceManager에서 gpuResource생성요청

    // GRM::GRMPtr ptextureResource = mGpuResourceManager->CreateTexture(textureDesc);
    // if (ptextureResource.getResource() == nullptr)
    //{
    //     // log
    //     return nullptr;
    // }

    //// 성공시
    //// Texture인스턴스생성,적절하게 속성값 설정 , 리턴

    //// CreateTextureInstance(name);	//name은 수정될수있다.
    // Texture *tex = CreateTextureInstance(name, path);

    // if (tex == nullptr)
    //{
    //     // log
    //     // GpuResource 반납  - > RAII를 사용하여 반납을 자동으로 처리하자
    //     return nullptr;
    // }
    // tex->SetGpuResource(ptextureResource);

    //// createTextureMetaData
    //// register metaData
    // SetTextureProperties(tex, textureDesc);

    // if (keepRawDataFlag)
    //{
    //     tex->SetRawData(std::make_unique<GRM::TextureDesc>(textureDesc),);
    // }

    // return tex; //(커스텀스마트포인터로 넘기는것이 좋을지도)

    return nullptr;
}

CoreAsset::Texture *CoreAsset::TextureManager::GetAsset(const std::string &path, const std::string &name) const
{

    const std::string internalName = path + name;

    Asset *asset = mGlobalAssetRegistrySystem->GetAsset(internalName);
    if (asset->GetType() != EAssetType::eTexture)
        return nullptr;

    Texture *texture = static_cast<Texture *>(asset);
    return texture;
}

CoreAsset::Texture *CoreAsset::TextureManager::GetAsset(CoreAsset::AssetID id) const
{

    Asset *asset = mGlobalAssetRegistrySystem->GetAsset(id);
    if (asset->GetType() != EAssetType::eTexture)
        return nullptr;

    Texture *texture = static_cast<Texture *>(asset);
    return texture;
}

// CoreAsset::Texture *CoreAsset::TextureManager::CreateTexture(const SerializedTexture &serializedTexture,
//                                                              const SerializedTextureRawData
//                                                              &serializedTextureRawData, const
//                                                              CoreAsset::AssetMetaData &metaData, const std::string
//                                                              &path)
//{
//
//     // gpuResourrceManager에서 gpuResource생성요청
//
//     GRM::GRMPtr ptextureResource = mGpuResourceManager->CreateTexture(serializedTextureRawData.mTextureRawData);
//     if (ptextureResource.getResource() == nullptr)
//     {
//         // log
//         return nullptr;
//     }
//
//     Texture *texture = CreateTextureInstance(serializedTexture.mAssetName, path, serializedTexture.mAssetID);
//
//     if (texture == nullptr)
//     {
//         // log
//         // GpuResource 반납  - > RAII를 사용하여 반납을 자동으로 처리하자
//         return nullptr;
//     }
//     texture->SetGpuResource(ptextureResource);
//
//     // createTextureMetaData
//     // register metaData
//     SetTextureProperties(texture, serializedTextureRawData.mTextureRawData);
//
//     if (metaData.mKeepRawDataFlag)
//     {
//         texture->SetRawData(std::make_unique<GRM::TextureDesc>(serializedTextureRawData.mTextureRawData));
//     }
//
//     return texture;
// }
//
// CoreAsset::Texture *CoreAsset::TextureManager::CreateTexture(const SerializedTexture &serializedTexture,
//                                                              SerializedTextureRawData &&serializedTextureRawData,
//                                                              const CoreAsset::AssetMetaData &metaData,
//                                                              const std::string &path)
//{
//
//     // gpuResourrceManager에서 gpuResource생성요청
//
//     GRM::GRMPtr ptextureResource = mGpuResourceManager->CreateTexture(serializedTextureRawData.mTextureRawData);
//     if (ptextureResource.getResource() == nullptr)
//     {
//         // log
//         return nullptr;
//     }
//
//     Texture *texture = CreateTextureInstance(serializedTexture.mAssetName, path, serializedTexture.mAssetID);
//
//     if (texture == nullptr)
//     {
//         // log
//         // GpuResource 반납  - > RAII를 사용하여 반납을 자동으로 처리하자
//         return nullptr;
//     }
//     texture->SetGpuResource(ptextureResource);
//
//     // createTextureMetaData
//     // register metaData
//     SetTextureProperties(texture, serializedTextureRawData.mTextureRawData);
//
//     if (metaData.mKeepRawDataFlag)
//     {
//         texture->SetRawData(std::make_unique<GRM::TextureDesc>(std::move(serializedTextureRawData.mTextureRawData)));
//     }
//
//     return texture;
// }

CoreAsset::Texture *CoreAsset::TextureManager::CreateTextureInstance(const std::string &name, const std::string &path)
{

    //// name 검사
    // const std::string &originalBaseName = name; //
    // std::string displayName = name;             // displayName은 ui에보여지는이름

    // std::string internalName = path + "/" + displayName; // 내부 유일성을보장하는 path+displayName
    //// bool hasName = mIDTable.HasName(displayName);
    // bool hasName = mGlobalAssetRegistrySystem->HasName(internalName);

    // if (hasName == true)
    //{
    //     // 중복된다면 조정
    //     // 새로운이름를찾는다.
    //     do
    //     {
    //         displayName = CoreUtility::Utility::MakeUniqueName(originalBaseName);
    //         internalName = path + "/" + displayName;

    //    } while (mGlobalAssetRegistrySystem->HasName(internalName));

    //    // log 이름이바뀌었다는사실을 남긴다.
    //}

    //// id get
    // AssetID newAssetID = mGlobalAssetRegistrySystem->GetNextAssetID();

    //// IDGenerator는 충돌없이 id를 만들어낼것이라 이런 검증코드는 필요없을수있다.

    //// 인스턴스생성
    // CoreAsset::Texture *texture = new CoreAsset::Texture(newAssetID, displayName);

    //// 등록
    // mGlobalAssetRegistrySystem->RegisterAsset(texture, internalName);
    //// mIDTable.SetAsset(internalName, newAssetID, texture);

    // return texture;
    return nullptr;
}

CoreAsset::Texture *CoreAsset::TextureManager::CreateTextureInstance(const std::string &name, const std::string &path,
                                                                     AssetID id)
{

    // std::string internalName = path + "/" + name;

    // CoreAsset::Texture *texture = new CoreAsset::Texture(id, name);

    // mGlobalAssetRegistrySystem->RegisterAsset(texture, internalName);

    // return texture;
    return nullptr;
}

void CoreAsset::TextureManager::SetTextureProperties(Texture *texture, const GRM::TextureDesc &textureDesc)
{

    texture->SetSize(textureDesc.mScratchImage.mMetadata.mWidth, textureDesc.mScratchImage.mMetadata.mHeight);
    texture->SetMipLevels(textureDesc.mScratchImage.mMetadata.mMipLevels);
    texture->SetTextureFormat(textureDesc.mScratchImage.mMetadata.mFormat);
    texture->SetTextureType(textureDesc.mScratchImage.mMetadata.mDimension);
    texture->SetSRGB(GRM::IsSRGBType(textureDesc.mScratchImage.mMetadata.mFormat));
}
