#include "FName.h"
#include <CoreBase/FNameTable.h>

FName::FName(const char *str)
{

    FNameTable *nameTable = FNameTable::GetInstance();
    mIndex = nameTable->GetIndex(str);
}

FName::~FName() {}

FName::FName(size_t index)
{

    FNameTable *nameTable = FNameTable::GetInstance();

    if (nameTable->IsValid(index) == false)
    {
        index = 0;
    }

    mIndex = index;
}

const char *FName::GetStr() const
{
    return nullptr;
}

size_t FName::GetIndex() const
{
    return mIndex;
}
