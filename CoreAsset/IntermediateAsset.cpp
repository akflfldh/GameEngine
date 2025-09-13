#include "CoreAsset/IntermediateAsset.h"
#include <BinaryReaderWriter/BinaryReader.h>
#include <BinaryReaderWriter/BinaryWriter.h>
// void CoreAsset::IntermediateAsset::Serialize(QuadRW::BinaryWriter &writer)
//{
//
//     writer.Write(mAssetID);
//     writer.Write((uint32_t)mAssetType);
//     writer.Write(mAssetName);
// }
//
// void CoreAsset::IntermediateAsset::DeSerialize(QuadRW::BinaryReader &reader)
//{
//
//     uint32_t type;
//
//     reader.Read(mAssetID);
//     reader.Read(type);
//     reader.Read(mAssetName);
//
//     mAssetType = (EAssetType)type;
// }
//
// void CoreAsset::IntermediateTexture::Serialize(QuadRW::BinaryWriter &writer)
//{
//
//     IntermediateAsset::Serialize(writer);
//     SerializeTextureProperties(mTextureProperties, writer);
//     writer.Write((unsigned int)mTextureUsage);
// }
//
// void CoreAsset::IntermediateTexture::DeSerialize(QuadRW::BinaryReader &reader)
//{
//     IntermediateAsset::DeSerialize(reader);
//     DeSerializeTextureProperties(mTextureProperties, reader);
//     unsigned int textureUsage;
//     reader.Read(textureUsage);
//     mTextureUsage = (GRM::ETextureUsage)textureUsage;
// }

// void CoreAsset::SerializedTextureRawData::SerializeImage(const GRM::Image &image, const uint8_t *mMemoryBase,
//                                                          QuadRW::BinaryWriter &writer)
//{
//     writer.Write(image.mWidth);
//     writer.Write(image.mHeight);
//     writer.Write((unsigned int)image.mFormat);
//     writer.Write(image.mRowPitch);
//     writer.Write(image.mSlicePitch);
//
//     // offset을 계산한다.
//     uintptr_t offset = (uintptr_t)(image.mPixels - mMemoryBase);
//     writer.Write(offset);
// }
//
// void CoreAsset::SerializedTextureRawData::SerializeScratchImage(const GRM::ScratchImage &scratchImage,
//                                                                 QuadRW::BinaryWriter &writer)
//{
//     writer.Write(scratchImage.mimagesNum);
//     writer.Write(scratchImage.mSize);
//     SerializeTextureMetaData(scratchImage.mMetadata, writer);
//
//     writer.Write((void *)scratchImage.mMemory.data(), scratchImage.mMemory.size());
//
//     for (const auto &image : scratchImage.mImages)
//     {
//         SerializeImage(image, scratchImage.mMemory.data(), writer);
//     }
// }
//
// void CoreAsset::SerializedTextureRawData::SerializeTextureMetaData(const GRM::TexMetaData &texMetaData,
//                                                                    QuadRW::BinaryWriter &writer)
//{
//
//     writer.Write(texMetaData.mWidth);
//     writer.Write(texMetaData.mHeight);
//     writer.Write(texMetaData.mArraySize);
//     writer.Write(texMetaData.mDepth);
//     writer.Write(texMetaData.mMipLevels);
//     writer.Write(texMetaData.mMiscFlags);
//     writer.Write(texMetaData.mMiscFlags2);
//     writer.Write((uint32_t)texMetaData.mFormat);
//     writer.Write((uint32_t)texMetaData.mDimension);
// }
//
// void CoreAsset::SerializedTextureRawData::DeSerializeImage(GRM::Image &oImage, uint8_t *mMemoryBase,
//                                                            QuadRW::BinaryReader &reader)
//{
//
//     reader.Read(oImage.mWidth);
//     reader.Read(oImage.mHeight);
//     unsigned int format;
//     reader.Read(format);
//     oImage.mFormat = (GRM::ETextureFormat)format;
//
//     reader.Read(oImage.mRowPitch);
//     reader.Read(oImage.mSlicePitch);
//
//     // offset을 이용하여 올바른 주소를 계산한다.
//     uintptr_t offset;
//     reader.Read(offset);
//
//     oImage.mPixels = mMemoryBase + offset;
// }
//
// void CoreAsset::SerializedTextureRawData::DeSerializeScratchImage(GRM::ScratchImage &oScratchImage,
//                                                                   QuadRW::BinaryReader &reader)
//{
//     reader.Read(oScratchImage.mimagesNum);
//     reader.Read(oScratchImage.mSize);
//     DeSerializeTextureMetaData(oScratchImage.mMetadata, reader);
//
//     // raw데이터복사
//     void *memory;
//     size_t size;
//     reader.Read(&memory, size);
//     oScratchImage.mMemory.resize(size);
//     memcpy(oScratchImage.mMemory.data(), memory, size);
//
//     oScratchImage.mImages.resize(oScratchImage.mimagesNum);
//     for (auto &image : oScratchImage.mImages)
//     {
//         DeSerializeImage(image, oScratchImage.mMemory.data(), reader);
//     }
// }
//
// void CoreAsset::SerializedTextureRawData::DeSerializeTextureMetaData(GRM::TexMetaData &oTexMetaData,
//                                                                      QuadRW::BinaryReader &reader)
//{
//
//     reader.Read(oTexMetaData.mWidth);
//     reader.Read(oTexMetaData.mHeight);
//     reader.Read(oTexMetaData.mArraySize);
//     reader.Read(oTexMetaData.mDepth);
//     reader.Read(oTexMetaData.mMipLevels);
//     reader.Read(oTexMetaData.mMiscFlags);
//     reader.Read(oTexMetaData.mMiscFlags2);
//
//     uint32_t format;
//     uint32_t dimension;
//
//     reader.Read(format);
//     reader.Read(dimension);
//
//     oTexMetaData.mFormat = (GRM::ETextureFormat)format;
//     oTexMetaData.mDimension = (GRM::ETextureType)dimension;
// }
//
// void CoreAsset::SerializedTextureRawData::Serialize(QuadRW::BinaryWriter &writer)
//{
//     writer.Write((unsigned int)mTextureRawData.mTextureUsage);
//     SerializeScratchImage(mTextureRawData.mScratchImage, writer);
// }
//
// void CoreAsset::SerializedTextureRawData::DeSerialize(QuadRW::BinaryReader &reader)
//{
//
//     unsigned int textureUsage;
//     reader.Read(textureUsage);
//     mTextureRawData.mTextureUsage = (GRM::ETextureUsage)textureUsage;
//
//     DeSerializeScratchImage(mTextureRawData.mScratchImage, reader);
// }
//
// void CoreAsset::SerializedTextureRuntime::Serialize(QuadRW::BinaryWriter &writer)
//{
//     SerializedTexture::Serialize(writer);
//     mSerializedRawData.Serialize(writer);
// }
//
// void CoreAsset::SerializeTextureProperties(const TextureProperties &mTextureProperties, QuadRW::BinaryWriter &writer)
//{
//     writer.Write(mTextureProperties.mWidth);
//     writer.Write(mTextureProperties.mHeight);
//     writer.Write(mTextureProperties.mMipLevels);
//     writer.Write((uint32_t)mTextureProperties.mTextureFormat);
//     writer.Write((uint32_t)mTextureProperties.mTextureType);
//     writer.Write(mTextureProperties.mIsSRGB);
// }
//
// void CoreAsset::DeSerializeTextureProperties(TextureProperties &oTextureProperties, QuadRW::BinaryReader &reader)
//{
//
//     reader.Read(oTextureProperties.mWidth);
//     reader.Read(oTextureProperties.mHeight);
//     reader.Read(oTextureProperties.mMipLevels);
//
//     uint32_t textureFormat;
//     uint32_t textureType;
//     reader.Read(textureFormat);
//     reader.Read(textureType);
//
//     oTextureProperties.mTextureFormat = (GRM::ETextureFormat)textureFormat;
//     oTextureProperties.mTextureType = (GRM::ETextureType)textureType;
//
//     reader.Read(oTextureProperties.mIsSRGB);
// }

// void CoreAsset::IntermediateMaterial::Serialize(QuadRW::BinaryWriter &writer)
//{
//     IntermediateAsset::Serialize(writer);
//     writer.Write(mGpuMaterialID);
//
//     writer.Write(mTexResourceList.size());
//     for (const auto &texResourceElement : mTexResourceList)
//     {
//         writer.Write(texResourceElement.first);  // name
//         writer.Write(texResourceElement.second); // aaset id
//     }
//
//     writer.Write(mSamplerResourceList.size());
//     for (auto samplerIDElement : mSamplerResourceList)
//     {
//         writer.Write(samplerIDElement);
//     }
// }

// void CoreAsset::IntermediateMaterial::DeSerialize(QuadRW::BinaryReader &reader)
//{
//     IntermediateAsset::DeSerialize(reader);
//     reader.Read(mGpuMaterialID);
//
//     size_t texResourceNum = 0;
//     reader.Read(texResourceNum);
//     mTexResourceList.resize(texResourceNum);
//     for (auto &texResourceElement : mTexResourceList)
//     {
//         reader.Read(texResourceElement.first);  // name
//         reader.Read(texResourceElement.second); // aaset id
//     }
//
//     size_t samplerNum;
//     reader.Read(samplerNum);
//
//     mSamplerResourceList.resize(samplerNum);
//     for (auto &samplerIDElement : mSamplerResourceList)
//     {
//         reader.Read(samplerIDElement);
//     }
// }

CoreAsset::IntermediateAssetFactory *CoreAsset::IntermediateAssetFactory::GetInstance()
{
    static IntermediateAssetFactory instance;
    return &instance;
}

CoreAsset::IntermediateAssetFactory::IntermediateAssetFactory() {}

CoreAsset::IntermediateAssetFactory::~IntermediateAssetFactory() {}

CoreAsset::IntermediateTexture *CoreAsset::IntermediateAssetFactory::CreateIntermediateTexture()
{
    return new IntermediateTexture;
}

CoreAsset::IntermediateMaterial *CoreAsset::IntermediateAssetFactory::CreateIntermediateMaterial()
{
    return new IntermediateMaterial;
}

void CoreAsset::IntermediateAssetFactory::ReleaseIntermediateAsset(IntermediateTexture *texture)
{
    if (texture == nullptr)
        return;
    delete texture;
}

void CoreAsset::IntermediateAssetFactory::ReleaseIntermediateAsset(IntermediateMaterial *material)
{

    if (material == nullptr)
        return;

    delete material;
}

void CoreAsset::IntermediateAssetFactory::ReleaseIntermediateAsset(CoreAsset::EAssetType type, IntermediateAsset *asset)
{

    switch (type)
    {
    case EAssetType::eTexture:
        return ReleaseIntermediateAsset(static_cast<IntermediateTexture *>(asset));

    case EAssetType::eMaterial:

        return ReleaseIntermediateAsset(static_cast<IntermediateMaterial *>(asset));
    }
}

CoreAsset::IntermediateMaterial::IntermediateMaterial()
{

    mAssetType = CoreAsset::EAssetType::eMaterial;
}

CoreAsset::IntermediateTexture::IntermediateTexture()
{

    mAssetType = CoreAsset::EAssetType::eTexture;
}

CoreAsset::IntermediateAsset::IntermediateAsset() {}
