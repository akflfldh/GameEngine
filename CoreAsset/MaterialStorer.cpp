#include "CoreAsset/MaterialStorer.h"

#include "CoreAsset/Material.h"
#include "CoreAsset/SerializedAssetType.h"
#include <BinaryReaderWriter/BinaryWriter.h>

CoreAsset::MaterialStorer *CoreAsset::MaterialStorer::GetInstance()
{
    static MaterialStorer instance;
    return &instance;
}

CoreAsset::MaterialStorer::MaterialStorer() {}

CoreAsset::MaterialStorer::~MaterialStorer() {}

bool CoreAsset::MaterialStorer::Store(CoreAsset::Material *asset, CoreAsset::AssetMetaData *metaData,
                                      const std::string &filePath)
{

    SerializedMaterial serializedMaterial;
    serializedMaterial.mAssetID = asset->GetID();
    serializedMaterial.mAssetType = asset->GetType();
    serializedMaterial.mAssetName = asset->GetName();
    serializedMaterial.mGpuMaterialID = asset->GetGpuMaterialID();

    const std::vector<AssetMaterialTexResourceContext> &texResourceInfoList = asset->GetTexResourceContextList();
    serializedMaterial.mTexResourceList.resize(texResourceInfoList.size());

    for (int i = 0; i < texResourceInfoList.size(); ++i)
    {
        serializedMaterial.mTexResourceList[i].first = texResourceInfoList[i].mTexShaderResourceInfo.mName;
        serializedMaterial.mTexResourceList[i].second = texResourceInfoList[i].mTexture.Get()->GetID();
    }

    const std::vector<uint32_t> samplerResourceInfoList = asset->GetSamplerResourceContextList();
    serializedMaterial.mSamplerResourceList.resize(samplerResourceInfoList.size());
    for (int i = 0; i < samplerResourceInfoList.size(); ++i)
    {
        serializedMaterial.mSamplerResourceList[i] = samplerResourceInfoList[i];
    }

    mWriter.StartWrite();
    serializedMaterial.Serialize(mWriter);

    // metaData기록여부
    bool metaDataFlag = true;
    if (metaData == nullptr)
        metaDataFlag = false;
    mWriter.Write(metaDataFlag);

    if (metaData)
        metaData->Serialize(mWriter);

    bool ret = mWriter.Close(filePath);
    if (ret == false)
    {
        // log
        return false;
    }
    return true;
}
