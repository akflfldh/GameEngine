#include "CoreAsset/Texture.h"
// #include <D3DGpuResourceManager/IGpuResource.h>

CoreAsset::Texture::Texture() : Asset(CoreAsset::EAssetType::eTexture) {}

CoreAsset::Texture::~Texture() {}

bool CoreAsset::Texture::CopyDataFrom(const Asset &source, std::string *failureReason)
{
    const Texture *sourceTexture = dynamic_cast<const Texture *>(&source);
    if (!sourceTexture)
    {
        if (failureReason)
            *failureReason = "Texture 에셋이 필요합니다.";
        return false;
    }
    if (this == sourceTexture)
        return Asset::CopyDataFrom(source, failureReason);

    const GRM::ScratchImage &image = sourceTexture->mProperties.mMetaData.mScratchImage;
    // 헤더만 로드된 경우 mPixels는 포인터가 아닌 offset이다. raw data 없이 복사 연산을 호출하지 않는다.
    if (image.mMemory.size() != image.mSize || image.mImages.size() != image.mimagesNum || image.mMemory.empty())
    {
        if (failureReason)
            *failureReason = "Texture의 pixel raw data와 image 목록을 먼저 준비해야 합니다.";
        return false;
    }
    const uintptr_t begin = reinterpret_cast<uintptr_t>(image.mMemory.data());
    for (const GRM::Image &subImage : image.mImages)
    {
        const uintptr_t pixels = reinterpret_cast<uintptr_t>(subImage.mPixels);
        if (pixels < begin || pixels - begin >= image.mMemory.size() ||
            subImage.mSlicePitch > image.mMemory.size() - (pixels - begin))
        {
            if (failureReason)
                *failureReason = "Texture image의 pixel 범위가 소유 메모리를 벗어났습니다.";
            return false;
        }
    }

    // ScratchImage의 기존 복사 연산이 pixel buffer를 깊게 복사하고 mPixels를 새 buffer에 재연결한다.
    mProperties = sourceTexture->mProperties;
    return Asset::CopyDataFrom(source, failureReason);
}

void CoreAsset::Texture::Serialize(Arch &arch)
{
    Asset::Serialize(arch);

    arch << mProperties;
}

void CoreAsset::Texture::SetRawData(uint8_t *pMemory, size_t size)
{
    mProperties.SetRawData(pMemory, size);
}

void CoreAsset::Texture::SetTextureDesc(const GRM::TextureDesc &texDesc)
{
    mProperties.mMetaData = texDesc;
}

const uint8_t *CoreAsset::Texture::GetRawData() const
{
    return mProperties.mMetaData.mScratchImage.mMemory.data();
}

const CoreAsset::TextureProperties &CoreAsset::Texture::GetProperties() const
{
    return mProperties;
}

CoreAsset::TextureProperties &CoreAsset::Texture::GetProperties()
{
    return mProperties;
    // TODO: 여기에 return 문을 삽입합니다.
}
