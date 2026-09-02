#include "BinaryReaderWriter/BinaryWriter.h"
#include <PhysicalFileSystem/PhysicalFileSystem.h>
QuadRW::BinaryWriter::BinaryWriter() : mPhysicalFileSystem(QuadPF::PhysicalFileSystem::GetInstance()), mWritePointer(0)
{
}

QuadRW::BinaryWriter::~BinaryWriter() {}

void QuadRW::BinaryWriter::StartWrite()
{
    mWritePointer = 0;
}
bool QuadRW::BinaryWriter::Close(const std::filesystem::path &filePath)
{

    bool ret = mPhysicalFileSystem->WriteBufferToFile(filePath, mBuffer.data(), mBuffer.size());
    mBuffer.clear();
    return ret;
}

bool QuadRW::BinaryWriter::Close()
{
    mBuffer.clear();
    return true;
}

void QuadRW::BinaryWriter::Write(const std::string &str)
{

    size_t size = str.size();
    Write(size);
    if (size != 0)
    {
        const uint8_t *pData = reinterpret_cast<const uint8_t *>(str.data());

        if (mBuffer.size() < mWritePointer + size)
        {
            mBuffer.resize(mWritePointer + size);
        }

        memcpy(&mBuffer[mWritePointer], pData, size);
        // mBuffer.insert(mBuffer.end(), pData, pData + size);
        mWritePointer += size;
    }
}

void QuadRW::BinaryWriter::Write(const void *data, size_t size)
{
    if (data == nullptr)
        return;

    Write(size);

    WriteRaw(data, size);
}

void QuadRW::BinaryWriter::WriteRaw(const void *data, size_t size)
{

    if (data == nullptr)
        return;

    const uint8_t *pData = reinterpret_cast<const uint8_t *>(data);
    mBuffer.insert(mBuffer.end(), pData, pData + size);

    mWritePointer += size;
}

// 일반 타입
template <typename T> void QuadRW::BinaryWriter::Write(T data)
{

    const uint8_t *pData = reinterpret_cast<const uint8_t *>(&data);

    if (mBuffer.size() < mWritePointer + sizeof(data))
    {
        mBuffer.resize(mWritePointer + sizeof(data));
    }

    memcpy(&mBuffer[mWritePointer], &data, sizeof(data));
    // mBuffer.insert(mBuffer.end(), pData, pData + sizeof(data));
    mWritePointer += sizeof(data);
}

void QuadRW::BinaryWriter::WriteRaw(const void *data, size_t size, size_t writePointer)
{

    if (mBuffer.size() < writePointer + size)
    {
        mBuffer.resize(writePointer + size);
    }
    memcpy(&mBuffer[writePointer], data, size);
}

uint64_t QuadRW::BinaryWriter::GetCurrentWritePointer() const
{

    return mWritePointer;
}

uint8_t *QuadRW::BinaryWriter::GetBufferPointer()
{
    return mBuffer.data();
}
size_t QuadRW::BinaryWriter::GetBufferSize() const
{
    return mBuffer.size();
}

template void QuadRW::BinaryWriter::Write<bool>(bool data);
template void QuadRW::BinaryWriter::Write<char>(char data);
template void QuadRW::BinaryWriter::Write<unsigned char>(unsigned char data);
template void QuadRW::BinaryWriter::Write<short>(short data);
template void QuadRW::BinaryWriter::Write<unsigned short>(unsigned short data);
template void QuadRW::BinaryWriter::Write<int>(int data);
template void QuadRW::BinaryWriter::Write<unsigned int>(unsigned int data);
template void QuadRW::BinaryWriter::Write<long>(long data);
template void QuadRW::BinaryWriter::Write<unsigned long>(unsigned long data);
template void QuadRW::BinaryWriter::Write<long long>(long long data);
template void QuadRW::BinaryWriter::Write<unsigned long long>(unsigned long long data);