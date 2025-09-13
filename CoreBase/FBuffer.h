#pragma once

#include <CoreBase/CoreBaseDllMacro.h>

#include <stdint.h>
#include <utility>

class COREBASE_API FBuffer
{
    friend void swap(FBuffer &lhs, FBuffer &rhs) noexcept;

  public:
    FBuffer(size_t size = 0);
    ~FBuffer();

    FBuffer(const FBuffer &lhs);
    FBuffer &operator=(const FBuffer &lhs);

    FBuffer(FBuffer &&lhs) noexcept;
    FBuffer &operator=(FBuffer &&lhs) noexcept;

    uint8_t &operator[](size_t index);

    const uint8_t *Data() const;
    uint8_t *Data();
    size_t Size() const;

  private:
    uint8_t *GetMemory(size_t size);
    void ReleaseMemory(void *ptr);

    void Swap(FBuffer &lhs) noexcept;

  private:
    uint8_t *mBuffer;
    size_t mSize;
};

inline void swap(FBuffer &lhs, FBuffer &rhs) noexcept
{
    lhs.Swap(rhs);
}
