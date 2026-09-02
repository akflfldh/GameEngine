#include "MaterialManager.h"
#include "GlobalAssetRegistrySystem.h"
#include "Material.h"
#include <RenderSystem/IMaterialManager.h>
#include <Utility/Utility.h>

#include "AssetMetaDataType.h"
#include "TextureManager.h"

CoreAsset::MaterialManager *CoreAsset::MaterialManager::GetInstance()
{
    static MaterialManager instance;
    return &instance;
}

CoreAsset::MaterialManager::MaterialManager()
{

    mGlobalAssetRegistrySystem = GlobalAssetRegistrySystem::GetInstance();
}

CoreAsset::MaterialManager::~MaterialManager() {}

CoreAsset::Material *CoreAsset::MaterialManager::CreateMaterial(Render::MaterialID gpuMaterialID,
                                                                const std::string &name, const std::string &path)
{

    Material *newMaterial = CreateMaterialInstance(name, path);

    newMaterial->mGpuMaterialID = gpuMaterialID;

    Render::IMaterialManager *gpuMaterialManager = Render::IMaterialManager::GetInstance();

    const Render::ShaderResourceInfoSet &shaderResourceInfo =
        gpuMaterialManager->GetMaterialShaderResourceInfo(gpuMaterialID);

    newMaterial->mTexResourceContextList.resize(shaderResourceInfo.mObjectTextureShaderResourceInfoVector.size());
    newMaterial->mSamplerResourceContextList.resize(shaderResourceInfo.mObjectSamplerShaderResourceInfoVector.size());

    for (int i = 0; i < newMaterial->mTexResourceContextList.size(); ++i)
    {

        newMaterial->mTexResourceContextList[i].mTexShaderResourceInfo =
            shaderResourceInfo.mObjectTextureShaderResourceInfoVector[i];
        newMaterial->mTexResourceContextList[i].mTexture = nullptr;
    }

    return newMaterial;
}

CoreAsset::Material *CoreAsset::MaterialManager::GetAsset(AssetID id) const
{

    Asset *asset = mGlobalAssetRegistrySystem->GetAsset(id);
    if (asset->GetType() != EAssetType::eMaterial)
        return nullptr;

    Material *material = static_cast<Material *>(asset);
    return material;
}

CoreAsset::Material *CoreAsset::MaterialManager::GetAsset(const std::string &name) const
{
    Asset *asset = mGlobalAssetRegistrySystem->GetAsset(name);
    if (asset->GetType() != EAssetType::eMaterial)
        return nullptr;

    Material *material = static_cast<Material *>(asset);
    return material;
}

// CoreAsset::Material *CoreAsset::MaterialManager::CreateMaterial(const CoreAsset::SerializedMaterial
// &serializedMaterial,
//                                                                 const CoreAsset::AssetMetaData &metaData,
//                                                                 const std::string &path)
//{
//     return nullptr;
//
//     // Material *material = CreateMaterialInstance(serializedMaterial.mAssetName, path, serializedMaterial.mAssetID);
//
//     // material->mGpuMaterialID = serializedMaterial.mGpuMaterialID;
//
//     // material->mTexResourceContextList.resize(serializedMaterial.mTexResourceList.size());
//     // material->mSamplerResourceContextList.resize(serializedMaterial.mSamplerResourceList.size());
//
//     // TextureManager *textureManager = TextureManager::GetInstance();
//     // Render::IMaterialManager *gpuMaterialManager = Render::IMaterialManager::GetInstance();
//
//     // const Render::ShaderResourceInfoSet &shaderResourceInfo =
//     //     gpuMaterialManager->GetMaterialShaderResourceInfo(material->mGpuMaterialID);
//
//     // for (int i = 0; i < material->mTexResourceContextList.size(); ++i)
//     //{
//     //     material->mTexResourceContextList[i].mTexture =
//     //         textureManager->GetAsset(serializedMaterial.mTexResourceList[i].second);
//     //     material->mTexResourceContextList[i].mTexShaderResourceInfo =
//     //         shaderResourceInfo.mTextureShaderResourceInfoVector[i];
//     // }
//
//     // for (int i = 0; i < material->mSamplerResourceContextList.size(); ++i)
//     //{
//     //     material->mSamplerResourceContextList[i] = serializedMaterial.mSamplerResourceList[i];
//     // }
//
//     // return material;
// }

CoreAsset::Material *CoreAsset::MaterialManager::CreateMaterialInstance(const std::string &name,
                                                                        const std::string &path)
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

    // AssetID assetID = mGlobalAssetRegistrySystem->GetNextAssetID();
    // Material *mat = new Material(assetID, name);

    // mGlobalAssetRegistrySystem->RegisterAsset(mat, internalName);

    // return mat;

    return nullptr;
}

CoreAsset::Material *CoreAsset::MaterialManager::CreateMaterialInstance(const std::string &name,
                                                                        const std::string &path, AssetID id)
{
    /* std::string internalName = path + "/" + name;

     Material *mat = new Material(id, name);

     mGlobalAssetRegistrySystem->RegisterAsset(mat, internalName);

     return mat;*/

    return nullptr;
}
