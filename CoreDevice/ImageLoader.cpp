#ifdef D3DX

#define _WIN32

#endif
#include "ImageLoader.h"
#include <CoreDevice/CoreDeviceHelper.h>
#include <Include/DirectTexX/DirectXTex.h>

Core::ImageLoader::~ImageLoader() {}

GRM::ScratchImage Core::ImageLoader::LoadFromMemory(const void *data, size_t size)
{

#ifdef D3DX

    DirectX::TexMetadata texMetaData;
    DirectX::ScratchImage d3dScratchImage;
    HRESULT ret = DirectX::LoadFromWICMemory((const uint8_t *)data, size, DirectX::WIC_FLAGS_NONE, &texMetaData,
                                             d3dScratchImage, nullptr);

    if (FAILED(ret))
    {
        return {};
    }

    // 텍스처데이터를위한 메타데이터를 채운다
    GRM::ScratchImage grmScratchImage;
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

    return grmScratchImage;

#endif
}

GRM::ScratchImage Core::ImageLoader::LoadFromFile(const std::filesystem::path &filePath)
{
#ifdef D3DX

    DirectX::TexMetadata texMetaData;
    DirectX::ScratchImage d3dScratchImage;
    HRESULT ret =
        DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_NONE, &texMetaData, d3dScratchImage, nullptr);

    if (FAILED(ret))
    {
        return {};
    }

    // 텍스처데이터를위한 메타데이터를 채운다
    GRM::ScratchImage grmScratchImage;
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

    return grmScratchImage;

#endif

    return GRM::ScratchImage();
}
