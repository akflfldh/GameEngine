#include "GpuTypes.h"

bool GRM::IsSRGBType(ETextureFormat format)
{
    switch (format)
    {
    case ETextureFormat::eR8G8B8A8_UNORM_SRGB:
    case ETextureFormat::eB8G8R8A8_UNORM_SRGB:
    case ETextureFormat::eBC1_UNORM_SRGB:
    case ETextureFormat::eBC2_UNORM_SRGB:
    case ETextureFormat::eBC3_UNORM_SRGB:
    case ETextureFormat::eBC7_UNORM_SRGB:
        return true;

    default:
        return false;
    }
}





GRM::ScratchImage::ScratchImage(const ScratchImage& rhs)
{
    mimagesNum = rhs.mimagesNum;
    mSize = rhs.mSize;
    mMetadata = rhs.mMetadata;

    mImages.resize(mimagesNum);
    mMemory.resize(mSize);

    for (int i = 0; i < rhs.mimagesNum; ++i)
    {
        mImages[i] = rhs.mImages[i];

        //offset 
        uintptr_t offset = rhs.mImages[i].mPixels - rhs.mMemory.data();
        mImages[i].mPixels = mMemory.data() + offset;
    }

}

GRM::ScratchImage& GRM::ScratchImage::operator=(const GRM::ScratchImage& rhs)
{
    mimagesNum = rhs.mimagesNum;
    mSize = rhs.mSize;
    mMetadata = rhs.mMetadata;

    mImages.resize(mimagesNum);
    mMemory.resize(mSize);
    for (int i = 0; i < rhs.mimagesNum; ++i)
    {
        mImages[i] = rhs.mImages[i];

        //offset 
        uintptr_t offset = rhs.mImages[i].mPixels - rhs.mMemory.data();
        mImages[i].mPixels = mMemory.data() + offset;
    }

    return *this;


}