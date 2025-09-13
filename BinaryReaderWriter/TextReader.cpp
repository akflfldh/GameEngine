#include "TextReader.h"
#include <PhysicalFileSystem/PhysicalFileSystem.h>
QuadRW::TextReader::TextReader() : mPhysicalFileSystem(QuadPF::PhysicalFileSystem::GetInstance()) {}

QuadRW::TextReader::~TextReader() {}

bool QuadRW::TextReader::StartRead(const char *filePath)
{
    FlushBuffer();
    mReadPointer = 0;
    return mPhysicalFileSystem->ReadFileToBuffer(filePath, mBuffer);
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
