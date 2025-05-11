#include "EffectUtility.h"

int Quad::EffectUtility::ConvertObjectOrPassFlag(const std::wstring& flag)
{
    if (flag == L"Object")
    {
        return 0;
    }else if (flag == L"Pass")
    {
        return 1;
    }
    else
    {
        return -1;
    }

}
