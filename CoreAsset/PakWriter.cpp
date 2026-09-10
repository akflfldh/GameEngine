#include "PakWriter.h"
#include "BinaryReaderWriter/BinaryWriter.h"
#include <PhysicalFileSystem/PhysicalFileSystem.h>
PakWriter::PakWriter() {}

PakWriter::~PakWriter() {}

bool PakWriter::StorePak(const std::filesystem::path &outputPakPath,
                         const std::vector<PakWriteAssetRecord> &pakWriteAssetRecord)
{

    auto physicalFileSystem = QuadPF::PhysicalFileSystem::GetInstance();

    // header
    PakHeader header;
    header.mMagic[0] = 'Q';
    header.mMagic[1] = 'P';
    header.mMagic[2] = 'A';
    header.mMagic[3] = 'K';

    // entry table 구축

    // registry list 구축

    std::vector<PakEntry> entryList;
    uint64_t entryCount = 0;
    std::vector<std::string> registryList;
    std::vector<uint64_t> registryPathRelativeOffsetList;

    QuadRW::BinaryWriter binaryWriter;
    binaryWriter.StartWrite();
    for (size_t i = 0; i < pakWriteAssetRecord.size(); ++i)
    {
        entryCount++;
        registryList.push_back(pakWriteAssetRecord[i].mRegistryPath);
        registryPathRelativeOffsetList.push_back(binaryWriter.GetCurrentWritePointer());
        binaryWriter.Write(pakWriteAssetRecord[i].mRegistryPath);
    }

    size_t stringTableSize = binaryWriter.GetBufferSize();

    binaryWriter.Close();

    for (size_t i = 0; i < pakWriteAssetRecord.size(); ++i)
    {

        if (pakWriteAssetRecord[i].mRawDataPath != "")
        {
            entryCount++;
        }
    }

    header.mEntryCount = entryCount;
    header.mTableOffset = GetHeaderSize();
    header.mRegistryPathStringTableOffset = header.mTableOffset + GetEntrySize() * entryCount;
    header.mDataOffset = header.mRegistryPathStringTableOffset + stringTableSize;
    header.mVersion = 1;
    // 여기까지

    uint64_t dataOffset = header.mDataOffset;
    std::vector<std::vector<uint8_t>> serializedBufferList;
    for (size_t i = 0; i < pakWriteAssetRecord.size(); ++i)
    {
        std::vector<uint8_t> buffer;
        physicalFileSystem->ReadFileToBuffer(pakWriteAssetRecord[i].mSerializedAssetPath, buffer);

        PakEntry entry{};
        entry.mAssetID = pakWriteAssetRecord[i].mAssetID;
        entry.mAssetType = static_cast<uint32_t>(pakWriteAssetRecord[i].mAssetType);
        entry.mPayloadType = EPakPayloadType::eSerializedAsset;
        entry.mFlags = pakWriteAssetRecord[i].mFlags;
        entry.mRegistryPathOffset = header.mRegistryPathStringTableOffset + registryPathRelativeOffsetList[i];
        entry.mDataOffset = dataOffset;
        entry.mStoredSize = buffer.size();
        entry.mOriginalSize = buffer.size();

        dataOffset += buffer.size();

        entryList.push_back(entry);
        serializedBufferList.push_back(std::move(buffer));
    }

    std::vector<std::vector<uint8_t>> rawDataBufferList;
    for (size_t i = 0; i < pakWriteAssetRecord.size(); ++i)
    {

        if (pakWriteAssetRecord[i].mRawDataPath != "")
        {

            std::vector<uint8_t> buffer;
            physicalFileSystem->ReadFileToBuffer(pakWriteAssetRecord[i].mRawDataPath, buffer);

            PakEntry entry{};
            entry.mAssetID = pakWriteAssetRecord[i].mAssetID;
            entry.mAssetType = static_cast<uint32_t>(pakWriteAssetRecord[i].mAssetType);
            entry.mPayloadType = EPakPayloadType::eRawData;
            entry.mFlags = pakWriteAssetRecord[i].mFlags;
            entry.mRegistryPathOffset = header.mRegistryPathStringTableOffset + registryPathRelativeOffsetList[i];
            entry.mDataOffset = dataOffset;
            entry.mStoredSize = buffer.size();
            entry.mOriginalSize = buffer.size();

            dataOffset += buffer.size();
            entryList.push_back(entry);
            rawDataBufferList.push_back(std::move(buffer));
        }
    }

    header.mEntryCount = entryList.size();

    QuadRW::BinaryWriter writer;
    writer.StartWrite();

    for (int i = 0; i < 4; ++i)
    {
        writer.Write<char>(header.mMagic[i]);
    }
    // uint32_t mEntryCount;
    // uint64_t mTableOffset;
    // uint64_t mRegistryPathStringTableOffset;
    // uint64_t mDataOffset;
    writer.Write<uint32_t>(header.mVersion);

    writer.Write<uint32_t>(header.mEntryCount);
    writer.Write<uint64_t>(header.mTableOffset);
    writer.Write<uint64_t>(header.mRegistryPathStringTableOffset);
    writer.Write<uint64_t>(header.mDataOffset);

    for (size_t i = 0; i < entryList.size(); ++i)
    {
        const PakEntry &entry = entryList[i];

        writer.Write<CoreAsset::AssetID>(entry.mAssetID);
        writer.Write<uint32_t>(entry.mAssetType);
        writer.Write<uint8_t>(static_cast<uint8_t>(entry.mPayloadType));
        writer.Write<uint32_t>(entry.mFlags);
        writer.Write<uint64_t>(entry.mRegistryPathOffset);
        writer.Write<uint64_t>(entry.mDataOffset);
        writer.Write<uint64_t>(entry.mStoredSize);
        writer.Write<uint64_t>(entry.mOriginalSize);
    }

    // string list

    for (size_t i = 0; i < registryList.size(); ++i)
    {
        writer.Write(registryList[i]);
    }

    //   data
    for (size_t i = 0; i < serializedBufferList.size(); ++i)
    {

        std::vector<uint8_t> &buffer = serializedBufferList[i];

        writer.WriteRaw(buffer.data(), buffer.size());
    }

    for (size_t i = 0; i < rawDataBufferList.size(); ++i)
    {

        std::vector<uint8_t> &buffer = rawDataBufferList[i];

        writer.WriteRaw(buffer.data(), buffer.size());
    }

    return writer.Close(outputPakPath);
}

uint64_t PakWriter::GetHeaderSize() const
{
    PakHeader header;
    return sizeof(header.mMagic) + sizeof(header.mDataOffset) + sizeof(header.mEntryCount) +
           sizeof(header.mRegistryPathStringTableOffset) + sizeof(header.mTableOffset) + sizeof(header.mVersion);
}

uint64_t PakWriter::GetEntrySize() const
{
    return sizeof(CoreAsset::AssetID) + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint64_t) * 4;
}
