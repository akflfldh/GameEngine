#pragma once

struct SRECT
{
    float mLeft;
    float mRight;
    float mTop;
    float mBottom;

    bool operator==(const SRECT &rhs) const
    {
        if (mLeft != rhs.mLeft)
            return false;
        if (mRight != rhs.mRight)
            return false;
        if (mTop != rhs.mTop)
            return false;
        if (mBottom != rhs.mBottom)
            return false;
        return true;
    }

    bool operator!=(const SRECT &rhs) const
    {
        return !operator==(rhs);
    }
};


struct AABB
{

};