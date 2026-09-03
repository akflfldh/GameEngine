#include "CoreAsset/Material.h"

void CoreAsset::AssetMaterialTexResourceContext::Serialize(Arch &arch)
{
    CoreAsset::AssetID id = 0;
    arch << mTexture;

    // if (arch.GetLoadingFlag())
    //{
    //     //      arch << id;
    //     //        mTexture = id;

    //    arch << mTexture;
    //}
    // else
    //{
    //    arch << mTexture;

    //    // id = mTexture.GetAssetID();
    //    // arch << id;
    //}
}

Arch &operator<<(Arch &arch, CoreAsset::AssetMaterialTexResourceContext &assetMaterialTexResourceContext)
{
    CoreAsset::AssetID id = 0;

    arch << assetMaterialTexResourceContext.mTexture;

    /*  if (arch.GetLoadingFlag())
      {
          arch << id;
          assetMaterialTexResourceContext.mTexture = id;
      }
      else
      {
          id = assetMaterialTexResourceContext.mTexture.GetAssetID();
          arch << id;
      }*/

    return arch;
}

CoreAsset::Material::Material(AssetID id) : Asset(EAssetType::eMaterial, id), mUploadDirty(true) {}

CoreAsset::Material::~Material() {}

void CoreAsset::Material::SetMaterialHandle(uint32_t handle)
{
    mMaterialHandle = handle;
}
uint32_t CoreAsset::Material::GetMaterialHandle() const
{
    return mMaterialHandle;
}

const std::vector<CoreAsset::AssetMaterialTexResourceContext> &CoreAsset::Material::GetTexResourceContextList() const
{
    return mTexResourceContextList;
    // TODO: 여기에 return 문을 삽입합니다.
}

void CoreAsset::Material::SetTextureResource(int index, Texture *tex)
{
    if (mAlbedoResourceContextList.size() <= index)
        mAlbedoResourceContextList.resize(index + 1);

    mAlbedoResourceContextList[index].mTexture = tex;
}

void CoreAsset::Material::SetTextureResource(int index, CoreAsset::AssetID tex)
{

    if (mAlbedoResourceContextList.size() <= index)
        mAlbedoResourceContextList.resize(index + 1);

    mAlbedoResourceContextList[index].mTexture = tex;
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

    arch << mDiffuseColor;
    arch << mDiffuseFactor;
    arch << mSpecular;
    arch << mSpecularFactor;
    arch << mAmbient;
    arch << mRoughness;
    arch << mMetallic;
    arch << mShadingModel;
    arch << mUseExplicitGpuMaterial;

    arch << mAlbedoResourceContextList;
    arch << mNormalMapResourceContext;
    arch << mHasNormalMap;
    arch << mSamplerResourceContextList;
}

void CoreAsset::Material::SetGpuMaterial(Render::MaterialID id)
{

    mGpuMaterialID = id;
}

std::vector<CoreAsset::AssetMaterialTexResourceContext> &CoreAsset::Material::GetAlbedoTexResourceList()
{

    return mAlbedoResourceContextList;
}
const std::vector<CoreAsset::AssetMaterialTexResourceContext> &CoreAsset::Material::GetAlbedoTexResourceList() const
{
    CoreAsset::Material *mat = const_cast<CoreAsset::Material *>(this);

    return const_cast<const std::vector<CoreAsset::AssetMaterialTexResourceContext> &>(mat->GetAlbedoTexResourceList());
}

CoreAsset::AssetMaterialTexResourceContext &CoreAsset::Material::GetNormalTexResource()
{

    return mNormalMapResourceContext;
}

const CoreAsset::AssetMaterialTexResourceContext &CoreAsset::Material::GetNormalTexResource() const
{

    CoreAsset::Material *mat = const_cast<CoreAsset::Material *>(this);

    return const_cast<const CoreAsset::AssetMaterialTexResourceContext &>(mat->GetNormalTexResource());
}

void CoreAsset::Material::SetAlbedoMap(int index, Texture *texture)
{

    if (mAlbedoResourceContextList.size() <= index)
        return;

    mAlbedoResourceContextList[index].mTexture = texture;
}

void CoreAsset::Material::SetNormalMapTexture(Texture *tex)
{

    mHasNormalMap = true;
    mNormalMapResourceContext.mTexture = tex;
}

bool CoreAsset::Material::HasNormalMap() const
{

    return mHasNormalMap;
}
void CoreAsset::Material::SetHasNormalMap(bool flag)
{

    mHasNormalMap = flag;
}
void CoreAsset::Material::AddAlbedoTexSlot()
{
    AssetMaterialTexResourceContext context;

    Render::TextureShaderResourceInfo &info = context.mTexShaderResourceInfo;

    info.mFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM;
    info.mDimension = Render::EShaderResourceDimension::eTex2D;
    info.mType = Render::EShaderResourceType::eTexture;
    info.mName = "AlbedoTexture";

    mAlbedoResourceContextList.push_back({info, nullptr});
}

void CoreAsset::Material::SetMetallic(float value)
{
    SetUploadDirty();
    mMetallic = value;
}
void CoreAsset::Material::SetRoughness(float value)
{
    SetUploadDirty();
    mRoughness = value;
}

float CoreAsset::Material::GetMetallic() const
{

    return mMetallic;
}
float CoreAsset::Material::GetRoughness() const
{

    return mRoughness;
}

void CoreAsset::Material::SetUploadDirty()
{
    mUploadDirty = true;
}
void CoreAsset::Material::ClearUploadDirty()
{
    mUploadDirty = false;
}
bool CoreAsset::Material::GetUploadDirty() const
{

    return mUploadDirty;
}

void CoreAsset::Material::SetDiffuseColor(const CoreMath::Vector3 &color)
{

    mDiffuseColor = color;
}
CoreMath::Vector3 CoreAsset::Material::GetDiffuseColor() const
{

    return mDiffuseColor;
}

void CoreAsset::Material::SetDiffuseFactor(float factor)
{

    mDiffuseFactor = factor;
}
float CoreAsset::Material::GetDiffuseFactor() const
{

    return mDiffuseFactor;
}

void CoreAsset::Material::SetSpecular(const CoreMath::Vector3 &specular)
{

    mSpecular = specular;
}

void CoreAsset::Material::SetSpecularFactor(float factor)
{

    mSpecularFactor = factor;
}

void CoreAsset::Material::SetAmbient(const CoreMath::Vector3 &ambient)
{

    mAmbient = ambient;

    if (mAmbient.X < 0.0f)
        mAmbient.X = 0.0f;

    if (mAmbient.Y < 0.0f)
        mAmbient.Y = 0.0f;

    if (mAmbient.Z < 0.0f)
        mAmbient.Z = 0.0f;
}

CoreMath::Vector3 CoreAsset::Material::GetSpecular() const
{

    return mSpecular;
}
float CoreAsset::Material::GetSpecularFactor() const
{

    return mSpecularFactor;
}

CoreMath::Vector3 CoreAsset::Material::GetAmbient() const
{

    return mAmbient;
}

void CoreAsset::Material::SetUseExplicitGpuMaterial(bool flag)
{

    mUseExplicitGpuMaterial = flag;
}
bool CoreAsset::Material::GetUseExplicitGpuMaterial() const
{

    return mUseExplicitGpuMaterial;
}

void CoreAsset::Material::CopyProperty(Material &targetMaterial)
{

    targetMaterial.mDiffuseColor = mDiffuseColor;
    targetMaterial.mDiffuseFactor = mDiffuseFactor;
    targetMaterial.mSpecular = mSpecular;
    targetMaterial.mSpecularFactor = mSpecularFactor;
    targetMaterial.mRoughness = mRoughness;
    targetMaterial.mMetallic = mMetallic;

    targetMaterial.mAlbedoResourceContextList = mAlbedoResourceContextList;
    targetMaterial.mNormalMapResourceContext = mNormalMapResourceContext;

    targetMaterial.mHasNormalMap = mHasNormalMap;
}

void CoreAsset::Material::SetShadingModel(EShadingModel shadingModel)
{

    mShadingModel = shadingModel;
}

CoreAsset::EShadingModel CoreAsset::Material::GetShadingMode() const
{

    return mShadingModel;
}

const std::vector<uint32_t> &CoreAsset::Material::GetSamplerResourceContextList() const
{

    return mSamplerResourceContextList;
}
