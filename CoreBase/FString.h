#pragma once
#include <CoreBase/CoreBaseDllMacro.h>

#include <CoreBase/Arch.h>
#include <CoreBase/FBuffer.h>

#include <utility>

class COREBASE_API FString
{
    friend void swap(FString &lhs, FString &rhs) noexcept;

  public:
    FString(const char *str = "");
    ~FString();

    FString(const FString &lhs);
    FString &operator=(const FString &lhs);

    FString(FString &&lhs) noexcept;
    FString &operator=(FString &&lhs) noexcept;

    const char *c_str() const;
    size_t size() const; // null terminator 미포함

    size_t capacity() const; // 실제 할당된 메모리 크기

    FString &operator+=(const FString &rhs);
    FString &operator+=(char c);

    bool operator==(const FString &lhs) const;

    char &operator[](size_t index);
    const char &operator[](size_t index) const;

  private:
    char *Alloc(size_t size);
    void Release(char *ptr);

    size_t CalculateCapacity(size_t length);
    void Swap(FString &lhs) noexcept;

    // 공간예약
    void Reserve(size_t requiredSize);

  private:
    size_t mLength;
    size_t mCapacity;
    char *mStr;
};
inline void swap(FString &lhs, FString &rhs) noexcept
{
    lhs.Swap(rhs);
}

namespace std
{
template <> struct hash<FString>
{
    size_t operator()(const FString &fstring) const
    {

        // FNV-1a hash 알고리즘을 사용한다 -> 자세한내용 위키를참조하자
        // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

        size_t fnv_offset_basic = 0xcbf29ce484222325ULL;
        size_t fnv_prime = 0x100000001b3ULL;

        size_t hash = fnv_offset_basic;
        for (size_t i = 0; i < fstring.size(); ++i)
        {
            // 부호확장을 방지하기위해서 fnv-la알고리즘은 1바이트 를 인풋으로 하는 알고리즘이기에
            // unsigned char로 캐스팅후 size_t로 캐스팅
            unsigned char c = static_cast<unsigned char>(fstring[i]);
            hash = hash ^ static_cast<size_t>(c);
            hash = hash * fnv_prime;
        }

        return hash;
    }
};

} // namespace std