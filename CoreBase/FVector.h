#pragma once
#include <CoreAllocator/FMemory.h>
#include <assert.h>
#include <new>
#include <utility>

#include <CoreBase/CoreBaseDllMacro.h>

template <typename T> class FVector;

template <typename T> void Swap(FVector<T> &lhs, FVector<T> &rhs) noexcept;

template <typename T> class FVector
{
    friend void Swap(FVector<T> &lhs, FVector<T> &rhs) noexcept;

  public:
    FVector();
    ~FVector();

    FVector(size_t size, const T &initValue);

    FVector(const FVector &lhs);
    FVector &operator=(const FVector &lhs);
    FVector(FVector &&lhs) noexcept;
    FVector &operator=(FVector &&lhs) noexcept;

    void Push_Back(const T &lhs);
    void Push_Back(T &&lhs);

    const T &operator[](size_t index) const;
    T &operator[](size_t index);

    size_t Size() const;

  private:
    void *Alloc(size_t size);
    void Release(void *ptr);

    // 메모리공간재할당 + 원소이동
    void ReAllocate(size_t size);

    void Swap(FVector<T> &rhs) noexcept;

  private:
    T *mBuffer;
    size_t mCapacity;
    size_t mSize;
};

template <typename T> void Swap(FVector<T> &lhs, FVector<T> &rhs) noexcept
{
    lhs.Swap(rhs);
}

template <typename T> inline void FVector<T>::Swap(FVector<T> &rhs) noexcept
{
    std::swap(mBuffer, rhs.mBuffer);
    std::swap(mCapacity, rhs.mCapacity);
    std::swap(mSize, rhs.mSize);
}

template <typename T> inline FVector<T>::FVector() : mBuffer(nullptr), mCapacity(0), mSize(0) {}

template <typename T> inline FVector<T>::~FVector()
{

    // 원소들의 소멸자 호출

    for (size_t i = 0; i < mSize; ++i)
    {
        mBuffer[i].~T();
    }

    Release(mBuffer);
}

template <typename T> inline FVector<T>::FVector(size_t size, const T &initValue)
{

    size_t capacity = size * 2;

    T *buffer = static_cast<T *>(Alloc(capacity));

    if (buffer == nullptr)
    {
        // exception
        throw std::bad_alloc();
    }

    size_t index = 0;
    try
    {

        for (index = 0; index < size; ++index)
        {
            new (&buffer[index]) T(initValue);
        }
    }
    catch (...)
    {
        // 혹시 원소복사생성자가 예외를 던지면
        // 작업중이던 buffer 해제
        // 원본은 오염되지않는다.
        for (size_t i = 0; i < index; ++i)
        {
            buffer[i].~T();
        }
        Release(buffer);
        throw;
    }

    mBuffer = buffer;
    mCapacity = capacity;
    mSize = size;
}

template <typename T> inline FVector<T>::FVector(const FVector &lhs) : mBuffer(nullptr), mCapacity(0), mSize(0)
{

    mBuffer = static_cast<T *>(Alloc(lhs.mCapacity));
    if (mBuffer == nullptr)
    {
        // throw exception
    }
    mCapacity = lhs.mCapacity;
    mSize = lhs.mSize;

    for (size_t i = 0; i < mSize; ++i)
    {
        new (&mBuffer[i]) T(lhs.mBuffer[i]);
    }
}

template <typename T> inline FVector<T> &FVector<T>::operator=(const FVector<T> &lhs)
{
    // copy and swap

    FVector<T> temp(lhs);
    Swap(*this, temp);

    return *this;
}

template <typename T> inline FVector<T>::FVector(FVector &&lhs) noexcept : FVector()
{
    this->Swap(lhs);
}

template <typename T> inline FVector<T> &FVector<T>::operator=(FVector<T> &&lhs) noexcept
{
    // swap
    this->Swap(lhs);
    return *this;
}

template <typename T> inline void FVector<T>::Push_Back(const T &lhs)
{

    if (mSize >= mCapacity)
    {
        // 메모리 재할당 필요
        ReAllocate(mCapacity);
    }

    // 맨뒤에 원소 복사생성
    new (&mBuffer[mSize++]) T(lhs);
}

template <typename T> inline void FVector<T>::Push_Back(T &&lhs)
{

    if (mSize >= mCapacity)
    {
        // 메모리 재할당 필요
        ReAllocate(mCapacity);
    }

    // 맨뒤에 원소 이동생성
    new (&mBuffer[mSize++]) T(std::move(lhs));
}

template <typename T> inline const T &FVector<T>::operator[](size_t index) const
{
    assert(index < mSize && "FVector index out of range ");
    return mBuffer[index];
}

template <typename T> inline T &FVector<T>::operator[](size_t index)
{

    assert(index < mSize && "FVector index out of range ");

    const FVector<T> &cFVector = *this;
    return const_cast<T &>(cFVector.operator[](index));
}

template <typename T> inline size_t FVector<T>::Size() const
{
    return mSize;
}

template <typename T> void *FVector<T>::Alloc(size_t size)
{
    // sizeof(T)
    size_t bufferSize = sizeof(T) * size;
    if (bufferSize == 0)
        return nullptr;

    // 엔진 allocator 호출
    CoreAlloc::FMemory *fMemory = CoreAlloc::FMemory::GetInstance();
    return fMemory->Allocate(bufferSize, alignof(T));
}

template <typename T> void FVector<T>::Release(void *ptr)
{

    // 엔진 Release호출
    CoreAlloc::FMemory *fMemory = CoreAlloc::FMemory::GetInstance();
    fMemory->Release(ptr);
}

template <typename T> inline void FVector<T>::ReAllocate(size_t size)
{

    size_t tempCapacity = size;
    if (tempCapacity < mCapacity)
        return;

    tempCapacity = tempCapacity == 0 ? 8 : tempCapacity * 2;

    T *newBuffer = static_cast<T *>(Alloc(tempCapacity));

    if (newBuffer == nullptr)
    {
        // 예외
        return;
    }
    // alloc

    // generate

    // 원소들을 이동시킨다.
    // 원소의 이동생성자는 noexcept로 선언되어있어야 안전이 보장된다.
    // 만약 그렇지않다면? 이동중 예외발생, 컨테이너가 오염된다.  따라서 복사생성으로 처리할수밖에
    for (size_t i = 0; i < mSize; ++i)
    {
        new (&newBuffer[i]) T(std::move(mBuffer[i]));
    }

    // release
    for (size_t i = 0; i < mSize; ++i)
    {
        mBuffer[i].~T();
    }

    Release(mBuffer);

    mBuffer = newBuffer;
}
