#pragma once
#include <functional>
namespace CoreUtility
{

class UniqueID
{
  public:
    UniqueID();
    ~UniqueID();

    UniqueID &operator=(const UniqueID &) = default;
    UniqueID(const UniqueID &) = default;

    UniqueID &operator=(UniqueID &&) = default;
    UniqueID(UniqueID &&) = default;

    bool operator==(const UniqueID &rhs) const;
    bool operator!=(const UniqueID &rhs) const;

    // map을위해 operator<

    bool operator<(const UniqueID &lhs) const;

    // unordered_map을 위해 hash 함수

    unsigned char mUniqueID[16] = {};
};

} // namespace CoreUtility

namespace std
{
template <> struct hash<CoreUtility::UniqueID>
{
    size_t operator()(const CoreUtility::UniqueID &id) const noexcept
    {

        const uint64_t *p = reinterpret_cast<const uint64_t *>(id.mUniqueID);

        uint64_t p0 = p[0];
        uint64_t p1 = p[1];

        return p0 ^ p1;
    };
};

} // namespace std