#include "Utility/UniqueID.h"
#include <Utility/uuid.h>
CoreUtility::UniqueID::UniqueID() {}

CoreUtility::UniqueID::~UniqueID() {}

bool CoreUtility::UniqueID::operator==(const UniqueID &rhs) const
{

    int ret = std::memcmp(mUniqueID, rhs.mUniqueID, 16);

    if (ret == 0)
        return true;
    return false;
}

bool CoreUtility::UniqueID::operator!=(const UniqueID &rhs) const
{
    return !operator==(rhs);
}

bool CoreUtility::UniqueID::operator<(const UniqueID &rhs) const
{

    // a<b이면 음수가 나오고 ->true ,그외에는  false
    return std::memcmp(mUniqueID, rhs.mUniqueID, 16) < 0;
}
