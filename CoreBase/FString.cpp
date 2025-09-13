#include "FString.h"
#include "FVector.h"
#include <cstring>
#include <utility>
#include <vector>
FString::FString(const char *str)
{
    if (str == nullptr)
    {
        mLength = 0;
        mCapacity = 0;
        mStr = nullptr;
    }
    else
    {
        mLength = std::strlen(str);
        mCapacity = CalculateCapacity(mLength);
        mStr = static_cast<char *>(Alloc(mCapacity));
        std::memcpy(mStr, str, mLength + 1);
    }
}

FString::~FString()
{
    Release(mStr);
}

FString::FString(const FString &lhs) : mLength(lhs.mLength)
{
    if (lhs.mLength != 0)
    {
        mCapacity = lhs.mCapacity;
        mStr = static_cast<char *>(Alloc(lhs.mCapacity));
        memcpy(mStr, lhs.mStr, lhs.mLength + 1);
    }
    else
    {
        mLength = 0;
        mCapacity = 0;
        mStr = nullptr;
    }
}

FString &FString::operator=(const FString &lhs)
{
    // copy and swap 패턴을 적용하자

    FString temp(lhs);

    Swap(temp);
    return *this;
}

FString::FString(FString &&lhs) noexcept : mStr(lhs.mStr), mLength(lhs.mLength), mCapacity(lhs.mCapacity)
{
    lhs.mLength = 0;
    lhs.mStr = nullptr;
    lhs.mCapacity = 0;
}

FString &FString::operator=(FString &&lhs) noexcept
{
    std::swap(mStr, lhs.mStr);
    std::swap(mLength, lhs.mLength);
    std::swap(mCapacity, lhs.mCapacity);
    return *this;
}

const char *FString::c_str() const
{
    return mStr;
}

size_t FString::size() const
{
    return mLength;
}

size_t FString::capacity() const
{
    return mCapacity;
}

FString &FString::operator+=(const FString &rhs)
{
    if (rhs.mLength == 0)
    {
        return *this;
    }

    if (mCapacity < (mLength + rhs.mLength + 1))
    {
        // 새로운 메모리 할당필요

        size_t requireLength = mLength + rhs.mLength;
        // 새로운크기
        size_t newCapacity = CalculateCapacity(requireLength);

        char *newStr = static_cast<char *>(Alloc(newCapacity));

        // 기존데이터 복사 (널문자 제외)
        memcpy(newStr, mStr, mLength);

        mCapacity = newCapacity;

        // 기존 메모리해제
        Release(mStr);

        mStr = newStr;
    }
    // 뒤에 이어서 복사 (널문자 포함)
    memcpy(&mStr[mLength], rhs.mStr, rhs.mLength + 1);
    mLength += rhs.mLength;

    return *this;
}

FString &FString::operator+=(char c)
{

    if (size() + 1 >= capacity())
    {
        // resize(require size );
        Reserve(mLength + 1);
    }

    mStr[mLength] = c;
    mStr[mLength + 1] = '\0';
    mLength++;
    return *this;
}

bool FString::operator==(const FString &lhs) const
{

    if (mLength != lhs.mLength)
        return false;

    if (mLength == 0)
        return true;

    return std::memcmp(mStr, lhs.mStr, mLength) == 0;
}

char &FString::operator[](size_t index)
{
    return mStr[index];
}
const char &FString::operator[](size_t index) const
{

    return mStr[index];
}

char *FString::Alloc(size_t size)
{
    if (size == 0)
        return nullptr;

    return new char[size];
}

void FString::Release(char *ptr)
{
    delete[] ptr;
}

size_t FString::CalculateCapacity(size_t length)
{
    return length + length / 2 + 1;
}

void FString::Swap(FString &lhs) noexcept
{
    std::swap(mStr, lhs.mStr);
    std::swap(mLength, lhs.mLength);
    std::swap(mCapacity, lhs.mCapacity);
}

void FString::Reserve(size_t requiredSize)
{
    size_t newCapacity = CalculateCapacity(requiredSize);
    char *temp = Alloc(newCapacity);

    // 기존데이터가있었다면 복사수행
    if (mStr != nullptr)
    {
        memcpy(temp, mStr, mLength + 1);
    }

    Release(mStr);
    mStr = temp;
    mCapacity = newCapacity;
}