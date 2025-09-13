#include "MaterialFactory.h"
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/IntermediateAsset.h >
#include <CoreAsset/Material.h>
#include <RenderSystem/IMaterialManager.h>

CoreAsset::MaterialFactory *CoreAsset::MaterialFactory::GetInstance()
{
    static MaterialFactory instance;
    return &instance;
}

CoreAsset::MaterialFactory::MaterialFactory() {}

CoreAsset::MaterialFactory::~MaterialFactory() {}

CoreAsset::Asset *CoreAsset::MaterialFactory::CreateEmptyAsset()
{
    return new Material;
}

CoreAsset::Asset *CoreAsset::MaterialFactory::CreateAssetFromData(const IntermediateAsset &intermediateAsset)
{

    // 중간데이터를 가지고 텍스처 에셋을 생성한다.

    const IntermediateMaterial &intermediateMaterial = static_cast<const IntermediateMaterial &>(intermediateAsset);

    // Texture Asset 생성
    Material *material = static_cast<Material *>(CreateEmptyAsset());
    if (material == nullptr)
    {
        return nullptr;
    }
    // 메타데이터
    material->SetName(intermediateAsset.mAssetName);
    material->SetGpuMaterial(intermediateMaterial.mGpuMaterialID);

    for (size_t i = 0; i < intermediateMaterial.mTexResourceList.size(); ++i)
    {
        material->SetTextureResource(i, intermediateMaterial.mTexResourceList[i].second);
    }

    for (size_t i = 0; i < intermediateMaterial.mSamplerResourceList.size(); ++i)
    {
        material->SetSamplerResource(i, intermediateMaterial.mSamplerResourceList[i]);
    }

    return material;
    //  return CreateMaterial(static_cast<const SerializedMaterial &>(serializedAsset), assetMetaData);
}

// CoreAsset::Asset *CoreAsset::MaterialFactory::CreateMaterial(const CoreAsset::SerializedMaterial &serializedMaterial,
//                                                              const CoreAsset::AssetMetaData &metaData)
//{
//     const std::string &path = metaData.mFilePath;
//
//     Material *material = CreateMaterialInstance(serializedMaterial.mAssetName, path, serializedMaterial.mAssetID);
//
//     material->mGpuMaterialID = serializedMaterial.mGpuMaterialID;
//
//     material->mTexResourceContextList.resize(serializedMaterial.mTexResourceList.size());
//     material->mSamplerResourceContextList.resize(serializedMaterial.mSamplerResourceList.size());
//
//     AssetManager *assetManager = AssetManager::GetInstance();
//     Render::IMaterialManager *gpuMaterialManager = Render::IMaterialManager::GetInstance();
//
//     const Render::ShaderResourceInfoSet &shaderResourceInfo =
//         gpuMaterialManager->GetMaterialShaderResourceInfo(material->mGpuMaterialID);
//
//     for (int i = 0; i < material->mTexResourceContextList.size(); ++i)
//     {
//         material->mTexResourceContextList[i].mTexture =
//             assetManager->GetAsset<Texture>(serializedMaterial.mTexResourceList[i].second);
//         material->mTexResourceContextList[i].mTexShaderResourceInfo =
//             shaderResourceInfo.mTextureShaderResourceInfoVector[i];
//     }
//
//     for (int i = 0; i < material->mSamplerResourceContextList.size(); ++i)
//     {
//         material->mSamplerResourceContextList[i] = serializedMaterial.mSamplerResourceList[i];
//     }
//
//     return material;
// }
