#include "TextReader.h"
#include <PhysicalFileSystem/PhysicalFileSystem.h>
#include <fstream>
#include <sstream>
QuadRW::TextReader::TextReader() : mPhysicalFileSystem(QuadPF::PhysicalFileSystem::GetInstance()) {}

QuadRW::TextReader::~TextReader() {}

bool QuadRW::TextReader::StartRead(const char *filePath)
{
    FlushBuffer();
    mReadPointer = 0;
    mStringStream.clear();

    std::ifstream fin(filePath);
    if (fin.is_open())
    {
        mStringStream << fin.rdbuf();
    }
    else
    {
        return false;
    }

    //    return mPhysicalFileSystem->ReadFileToBuffer(filePath, mBuffer);
}

bool QuadRW::TextReader ::StartRead(const std::string &filePath)
{

    return StartRead(filePath.c_str());
}

template <typename T> void QuadRW ::TextReader::Read(T &value)
{
    mStringStream >> value;

    //  std::string str = ss.str();

    // mBuffer.insert(mBuffer.end(), reinterpret_cast<const uint8_t *>(str.data()),
    //     reinterpret_cast<const uint8_t *>(str.data() + str.size()));
}

size_t QuadRW::TextReader::Read(void *oBuffer, size_t size)
{
    if (size == 0)
        return 0;

    const size_t remain = mBuffer.size() - mReadPointer;

    if (remain == 0)
    {
        return 0;
    }

    if (size > remain)
    {
        size = remain;
    }

    const uint8_t *pBufferPtr = &mBuffer[mReadPointer];
    memcpy(oBuffer, pBufferPtr, size);
    mReadPointer += size;
    return size;
}

void QuadRW::TextReader::FlushBuffer()
{
    mBuffer.clear();
}

uint64_t QuadRW::TextReader::GetFileSize() const
{
    return mBuffer.size();
}

bool QuadRW::TextReader::SetReadPointer(uint64_t pointerPos)
{
    if (pointerPos > mBuffer.size())
    {

        return false;
    }

    mReadPointer = pointerPos;
    return true;
}
uint64_t QuadRW::TextReader::GetReadPointer() const
{
    return mReadPointer;
}

bool QuadRW::TextReader::IsEOF() const
{
    return mReadPointer == GetFileSize() ? true : false;
}

template void QuadRW::TextReader::Read<char>(char &);
template void QuadRW::TextReader::Read<unsigned char>(unsigned char &);
template void QuadRW::TextReader::Read<short>(short &);
template void QuadRW::TextReader::Read<unsigned short>(unsigned short &);
template void QuadRW::TextReader::Read<int>(int &);
template void QuadRW::TextReader::Read<unsigned int>(unsigned int &);
template void QuadRW::TextReader::Read<long>(long &);
template void QuadRW::TextReader::Read<unsigned long>(unsigned long &);
template void QuadRW::TextReader::Read<long long>(long long &);
template void QuadRW::TextReader::Read<unsigned long long>(unsigned long long &);
template void QuadRW::TextReader::Read<float>(float &);
template void QuadRW::TextReader::Read<double>(double &);
template void QuadRW::TextReader::Read<bool>(bool &);
template void QuadRW::TextReader::Read<std::string>(std::string &);
