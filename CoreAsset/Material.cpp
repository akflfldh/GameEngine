#include "CoreAsset/Material.h"

void CoreAsset::AssetMaterialTexResourceContext::Serialize(Arch &arch) {}

CoreAsset::Material::Material(AssetID id) : Asset(EAssetType::eMaterial, id) {}

CoreAsset::Material::~Material() {}

const std::vector<CoreAsset::AssetMaterialTexResourceContext> &CoreAsset::Material::GetTexResourceContextList() const
{
    return mTexResourceContextList;
    // TODO: 여기에 return 문을 삽입합니다.
}

void CoreAsset::Material::SetTextureResource(int index, Texture *tex)
{
    if (mTexResourceContextList.size() <= index)
        mTexResourceContextList.resize(index + 1);

    mTexResourceContextList[index].mTexture = tex;
}

void CoreAsset::Material::SetTextureResource(int index, CoreAsset::AssetID tex)
{

    if (mTexResourceContextList.size() <= index)
        mTexResourceContextList.resize(index + 1);

    mTexResourceContextList[index].mTexture = tex;
}

void CoreAsset::Material::SetSamplerResource(int index, uint32_t samplerID)
{
    if (mSamplerResourceContextList.size() <= index)
        mSamplerResourceContextList.resize(index + 1);
    mSamplerResourceContextList[index] = samplerID;
}
void CoreAsset::Material::Serialize(Arch &arch)
{
    Asset::Serialize(arch);

    arch << mGpuMaterialID;

    // Render::MaterialID mGpuMaterialID = MaterialIDNone; // GPU 머터리얼 ID

    // text
    if (arch.GetLoadingFlag())
    {
        size_t texResourceNum = 0;
        arch << texResourceNum;

        if (texResourceNum != 0)
        {
            mTexResourceContextList.resize(texResourceNum);

            for (int i = 0; i < texResourceNum; ++i)
            {
                mTexResourceContextList[i].Serialize(arch);
            }
        }
    }
    else
    {
        size_t texResourceNum = mTexResourceContextList.size();
        arch << texResourceNum;
        if (texResourceNum != 0)
        {
            for (int i = 0; i < texResourceNum; ++i)
            {
                mTexResourceContextList[i].Serialize(arch);
            }
        }
    }

    // sampler id

    if (arch.GetLoadingFlag())
    {
        size_t samResourceNum = 0;
        arch << samResourceNum;

        if (samResourceNum != 0)
        {
            mSamplerResourceContextList.resize(samResourceNum);

            for (int i = 0; i < samResourceNum; ++i)
            {
                arch << mSamplerResourceContextList[i];
            }
        }
    }
    else
    {
        size_t samResourceNum = mSamplerResourceContextList.size();
        arch << samResourceNum;
        if (samResourceNum != 0)
        {
            for (int i = 0; i < samResourceNum; ++i)
            {
                arch << mSamplerResourceContextList[i];
            }
        }
    }
}

void CoreAsset::Material::SetGpuMaterial(Render::MaterialID id)
{

    mGpuMaterialID = id;
}

const std::vector<uint32_t> &CoreAsset::Material::GetSamplerResourceContextList() const
{

    return mSamplerResourceContextList;
}
