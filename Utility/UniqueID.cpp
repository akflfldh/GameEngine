#include "Utility/UniqueID.h"
#include <Utility/uuid.h>
CoreUtility::UniqueID::UniqueID() {}

CoreUtility::UniqueID::~UniqueID() {}

bool CoreUtility::UniqueID::operator==(const UniqueID &lhs) const
{

    int ret = std::memcmp(mUniqueID, lhs.mUniqueID, 16);

    if (ret == 0)
        return true;
    return false;
}

bool CoreUtility::UniqueID::operator!=(const UniqueID &lhs) const
{
    return !operator==(lhs);
}

bool CoreUtility::UniqueID::operator<(const UniqueID &lhs) const
{

    // a<b이면 음수가 나오고 ->true ,그외에는  false
    return std::memcmp(mUniqueID, lhs.mUniqueID, 16) < 0;
}
