

#ifdef D3DX
#include "FBXImporter.h"
#include "ImportModule/D3DHelper.h"
#include "ImportModule/TextureImporter.h"
#include <CommonHeader/GpuTypes.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/IntermediateAsset.h>
#include <CoreAsset/Texture.h>
#include <CoreAsset/TextureManager.h>
#include <D3DGpuResourceManager/D3DGpuType.h>
#include <DirectXTex.h>
#include <Logger/Logger.h>
#include <Utility/Utility.h>
#include <memory>

Import::TextureImporter *Import::TextureImporter::GetInstance()
{
    static TextureImporter instance;

    return &instance;
}

Import::TextureImporter::TextureImporter()
/*:mTextureManager(textureManager),mAssetMetaDataManager(CoreAsset::AssetMetaDataManager::GetInstance())*/
{
}

Import::TextureImporter::~TextureImporter() {}

CoreAsset::ImportPackage Import::TextureImporter::Import(
    const std::filesystem::path &path, CoreAsset::AssetImporterManager *assetImporterManagaer,
    const CoreAsset::ImportExecutionContext &executionContext) const
{

    // utility get extension

    GRM::TextureDesc textureDesc;

    // const std::wstring &filePathW = CoreUtility::Utility::ConvertToWString(filePath, true);

    DirectX::ScratchImage d3dScratchImage;
    DirectX::TexMetadata texMetaData;

    HRESULT result =
        DirectX::LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &texMetaData, d3dScratchImage);

    if (FAILED(result))
    {
        // log
        LOG_MESSAGE_INFO("TextureImporter", "LoadFromWICFile 실패");
        return {};
    }

    // 텍스처데이터를위한 메타데이터를 채운다
    GRM::ScratchImage &grmScratchImage = textureDesc.mScratchImage;
    uint8_t *d3dScratchImagePixels = d3dScratchImage.GetPixels(); // 픽셀데이터의 시작위치

    grmScratchImage.mSize = d3dScratchImage.GetPixelsSize();
    grmScratchImage.mMemory.resize(grmScratchImage.mSize);

    memcpy(grmScratchImage.mMemory.data(), d3dScratchImagePixels, grmScratchImage.mSize);

    grmScratchImage.mimagesNum = d3dScratchImage.GetImageCount();
    grmScratchImage.mMetadata.mWidth = texMetaData.width;
    grmScratchImage.mMetadata.mHeight = texMetaData.height;
    grmScratchImage.mMetadata.mDepth = texMetaData.depth;
    grmScratchImage.mMetadata.mArraySize = texMetaData.arraySize;
    grmScratchImage.mMetadata.mMipLevels = texMetaData.mipLevels;
    grmScratchImage.mMetadata.mDimension = ConvertTextureType(texMetaData.dimension);
    grmScratchImage.mMetadata.mFormat = ConvertToDxgiFormat(texMetaData.format);
    if (grmScratchImage.mMetadata.mFormat == GRM::ETextureFormat::eUnknown)
    {
        // log
        assert(0 && "grmScratchImage.mMetadata.mFormat == GRM::ETextureFormat::eUnknown");
        return {};
    }

    grmScratchImage.mMetadata.mMiscFlags = texMetaData.miscFlags;
    grmScratchImage.mMetadata.mMiscFlags2 = texMetaData.miscFlags2;

    grmScratchImage.mImages.resize(d3dScratchImage.GetImageCount());

    const DirectX::Image *d3dImages = d3dScratchImage.GetImages();

    for (int i = 0; i < d3dScratchImage.GetImageCount(); ++i)
    {
        uintptr_t ptrOffset = d3dImages[i].pixels - d3dScratchImagePixels;

        grmScratchImage.mImages[i].mPixels = &grmScratchImage.mMemory[ptrOffset];
        grmScratchImage.mImages[i].mWidth = d3dImages[i].width;
        grmScratchImage.mImages[i].mHeight = d3dImages[i].height;
        grmScratchImage.mImages[i].mRowPitch = d3dImages[i].rowPitch;
        grmScratchImage.mImages[i].mSlicePitch = d3dImages[i].slicePitch;
        grmScratchImage.mImages[i].mFormat = ConvertToDxgiFormat(d3dImages[i].format);
    }

    textureDesc.mTextureUsage = GRM::ETextureUsage::eShaderResource;
    // textureManager에게 ScratchImage 전달

    //   CoreAsset::IntermediateAssetFactory *intermediateAssetFactory =
    //   CoreAsset::IntermediateAssetFactory::GetInstance();
    std::unique_ptr<CoreAsset::IntermediateTexture> intermediateTexture =
        std::make_unique<CoreAsset::IntermediateTexture>();

    intermediateTexture->mAssetType = CoreAsset::EAssetType::eTexture;

    std::string name = path.filename().string();
    // CoreUtility::Utility::GetFileNameFromPath(filePath);
    name = CoreUtility::Utility::RemoveExtension(name);
    intermediateTexture->mAssetName = name.c_str();
    intermediateTexture->mTextureRawData = std::move(textureDesc);

    std::vector<std::unique_ptr<CoreAsset::IntermediateAsset>> retVector;
    retVector.push_back(std::move(intermediateTexture));

    CoreAsset::ImportPackage importPackage;
    importPackage.mInteremdiateAssets.resize(retVector.size());
    for (int i = 0; i < retVector.size(); ++i)
    {
        importPackage.mInteremdiateAssets[i].mIntermediateAsset = std::move(retVector[i]);
    }

    return importPackage;
}

#endif