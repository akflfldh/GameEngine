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

CoreAsset::Asset *CoreAsset::MaterialFactory::CreateEmptyAsset(EAssetType assetType)
{
    auto mat = new Material;
    mat->SetMaterialHandle(GetNextMaterialHandle());
    return mat;
}

CoreAsset::Asset *CoreAsset::MaterialFactory::CreateAssetFromData(const IntermediateAsset &intermediateAsset)
{

    // 중간데이터를 가지고 텍스처 에셋을 생성한다.

    const IntermediateMaterial &intermediateMaterial = static_cast<const IntermediateMaterial &>(intermediateAsset);

    // Texture Asset 생성
    Material *material = static_cast<Material *>(CreateEmptyAsset(EAssetType::eMaterial));
    if (material == nullptr)
    {
        return nullptr;
    }

    // 메타데이터
    //   material->SetName(intermediateAsset.mAssetName);
    material->SetGpuMaterial(intermediateMaterial.mGpuMaterialID);

    material->SetRoughness(1.0f - intermediateMaterial.mShininess);
    material->SetDiffuseColor(intermediateMaterial.mDiffuseColor);
    material->SetDiffuseFactor(intermediateMaterial.mDiffuseFactor);
    material->SetSpecular(intermediateMaterial.mSpecular);
    material->SetSpecularFactor(intermediateMaterial.mSpecularFactor);
    material->SetShadingModel(intermediateMaterial.mShadingModel);

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

uint32_t CoreAsset::MaterialFactory::GetNextMaterialHandle()
{

    if (mMaterialHandleIndexPool.empty())
        return mNextMaterialHandle++;

    uint32_t handle = mMaterialHandleIndexPool.back();
    mMaterialHandleIndexPool.pop_back();
    return handle;
}

void CoreAsset::MaterialFactory::ReleaseMaterialHandle(uint32_t handle)
{
    mMaterialHandleIndexPool.push_back(handle);
}
