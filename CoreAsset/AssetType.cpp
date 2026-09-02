#include <CoreAsset/AssetType.h>

Arch &CoreAsset::operator<<(Arch &arch, CoreAsset::TextureProperties &textureProperty)
{
    // 구별하지않아도 되는것
    arch << textureProperty.mIsSRGB;
    arch << textureProperty.mMetaData.mScratchImage.mimagesNum;
    arch << textureProperty.mMetaData.mScratchImage.mSize;
    arch << textureProperty.mMetaData.mScratchImage.mMetadata.mWidth;
    arch << textureProperty.mMetaData.mScratchImage.mMetadata.mHeight;
    arch << textureProperty.mMetaData.mScratchImage.mMetadata.mDepth;
    arch << textureProperty.mMetaData.mScratchImage.mMetadata.mArraySize;
    arch << textureProperty.mMetaData.mScratchImage.mMetadata.mMipLevels;
    arch << textureProperty.mMetaData.mScratchImage.mMetadata.mMiscFlags;
    arch << textureProperty.mMetaData.mScratchImage.mMetadata.mMiscFlags2;
    arch << textureProperty.mMetaData.hasOptimizedClearValue;

    // optimizedClearValue가 false이면 쓰레기값, 통일성을 위해서  .// 빅엔디안 ,리틀엔디안도 고려해야할지도
    arch << textureProperty.mMetaData.mOptimizedClearValue.mOptimizedColor[0];
    arch << textureProperty.mMetaData.mOptimizedClearValue.mOptimizedColor[1];
    arch << textureProperty.mMetaData.mOptimizedClearValue.mOptimizedColor[2];
    arch << textureProperty.mMetaData.mOptimizedClearValue.mOptimizedColor[3];

    // 구분할것
    if (arch.GetLoadingFlag())
    {
        // 로드
        uint8_t format = 0;
        uint8_t dimension = 0;

        arch << format << dimension;

        textureProperty.mMetaData.mScratchImage.mMetadata.mFormat = (GRM::ETextureFormat)format;
        textureProperty.mMetaData.mScratchImage.mMetadata.mDimension = (GRM::ETextureType)dimension;

        for (size_t i = 0; i < textureProperty.mMetaData.mScratchImage.mimagesNum; ++i)
        {
            GRM::Image image;
            uint8_t format = 0;
            uint64_t pixelOffset = 0;
            arch << format << image.mWidth << image.mHeight << image.mRowPitch << image.mSlicePitch << pixelOffset;

            image.mFormat = (GRM::ETextureFormat)format;
            image.mPixels = (uint8_t *)pixelOffset;

            textureProperty.mMetaData.mScratchImage.mImages.push_back(image);
        }

        uint16_t texUsage = 0;
        arch << texUsage;
        textureProperty.mMetaData.mTextureUsage = (GRM::ETextureUsage)texUsage;
    }
    else
    {
        // 저장
        uint8_t format = (uint8_t)textureProperty.mMetaData.mScratchImage.mMetadata.mFormat;
        uint8_t dimension = (uint8_t)textureProperty.mMetaData.mScratchImage.mMetadata.mDimension;
        arch << format << dimension;

        for (size_t i = 0; i < textureProperty.mMetaData.mScratchImage.mimagesNum; ++i)
        {

            GRM::Image &image = textureProperty.mMetaData.mScratchImage.mImages[i];
            uint8_t format = (uint8_t)image.mFormat;

            uint64_t pixelOffset = 0;
            if (textureProperty.mMetaData.mScratchImage.mMemory.empty())
            {
                // 로드되지않았으니, image pixel값들이 offset으로 유지되고있다.
                pixelOffset = (uint64_t)image.mPixels;
            }
            else
            {
                // offset을 게산한다.
                pixelOffset = image.mPixels - textureProperty.mMetaData.mScratchImage.mMemory.data();
            }

            arch << format << image.mWidth << image.mHeight << image.mRowPitch << image.mSlicePitch << pixelOffset;
        }

        uint16_t texUsage = (unsigned long)textureProperty.mMetaData.mTextureUsage;
        arch << texUsage;
    }

    return arch;
}

void CoreAsset::TextureProperties::SetRawData(uint8_t *pMemory, size_t size)
{
    if (pMemory == nullptr)
        return;

    mMetaData.mScratchImage.mMemory.resize(size);
    memcpy(mMetaData.mScratchImage.mMemory.data(), pMemory, size);

    // image들의 pixel값들을 offset에서 실제 ptr로

    for (auto &image : mMetaData.mScratchImage.mImages)
    {
        image.mPixels = (uint64_t)image.mPixels + mMetaData.mScratchImage.mMemory.data();
    }

    return;
}

