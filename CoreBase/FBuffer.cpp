#include "FBuffer.h"
#include <cstring>
FBuffer::FBuffer(size_t size) : mBuffer(nullptr), mSize(size)
{

    if (size != 0)
        mBuffer = GetMemory(size);
}

FBuffer::~FBuffer()
{
    ReleaseMemory(mBuffer);
}

FBuffer::FBuffer(const FBuffer &lhs) : mBuffer(GetMemory(lhs.mSize)), mSize(lhs.mSize)
{

    if (mBuffer == nullptr)
        return;

    memcpy(mBuffer, lhs.mBuffer, mSize * sizeof(uint8_t));
}

FBuffer &FBuffer::operator=(const FBuffer &lhs)
{

    // copy and swap패턴을 사용하자

    FBuffer temp(lhs);
    swap(*this, temp);

    return *this;
}

FBuffer::FBuffer(FBuffer &&lhs) noexcept : mBuffer(lhs.mBuffer), mSize(lhs.mSize)
{

    lhs.mBuffer = nullptr;
    lhs.mSize = 0;
}

FBuffer &FBuffer::operator=(FBuffer &&lhs) noexcept
{

    swap(*this, lhs);
    return *this;
}

uint8_t &FBuffer::operator[](size_t index)
{
    return mBuffer[index];
}

const uint8_t *FBuffer::Data() const
{

    return mBuffer;
}

uint8_t *FBuffer::Data()
{
    return mBuffer;
}

size_t FBuffer::Size() const
{

    return mSize;
}

uint8_t *FBuffer::GetMemory(size_t size)
{
    // 향후 커스텀할당자로부터 메모리를 할당받을것이다.
    if (size == 0)
        return nullptr;

    return new uint8_t[size];
}

void FBuffer::ReleaseMemory(void *ptr)
{
    delete[] static_cast<uint8_t *>(ptr);
}

void FBuffer::Swap(FBuffer &lhs) noexcept
{
    std::swap(mBuffer, lhs.mBuffer);
    std::swap(mSize, lhs.mSize);
}
