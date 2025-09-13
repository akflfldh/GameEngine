#include "BinaryReaderWriter/BinaryWriter.h"
#include <PhysicalFileSystem/PhysicalFileSystem.h>
QuadRW::BinaryWriter::BinaryWriter() : mPhysicalFileSystem(QuadPF::PhysicalFileSystem::GetInstance()) {}

QuadRW::BinaryWriter::~BinaryWriter() {}

void QuadRW::BinaryWriter::StartWrite()
{
    mBuffer.clear();
}
bool QuadRW::BinaryWriter::Close(const std::string &path)
{

    bool ret = mPhysicalFileSystem->WriteBufferToFile(path, mBuffer.data(), mBuffer.size());

    return ret;
}

void QuadRW::BinaryWriter::Write(const std::string &str)
{

    size_t size = str.size();
    Write(size);
    if (size != 0)
    {
        const uint8_t *pData = reinterpret_cast<const uint8_t *>(str.data());
        mBuffer.insert(mBuffer.end(), pData, pData + size);
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
}

// 일반 타입
template <typename T> void QuadRW::BinaryWriter::Write(T data)
{

    const uint8_t *pData = reinterpret_cast<const uint8_t *>(&data);

    mBuffer.insert(mBuffer.end(), pData, pData + sizeof(data));
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