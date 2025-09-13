#include "FNameTable.h"
#include <CoreBase/FNameTableImpl.h>

FNameTableImpl *FNameTable::mImpl = nullptr;

FNameTable *FNameTable::GetInstance()
{

    static FNameTable instance;

    return &instance;
}

FNameTable::FNameTable()
{
    mImpl = FNameTableImpl::GetInstance();
}

FNameTable::~FNameTable() {}

size_t FNameTable::GetIndex(const char *name) const
{
    return mImpl->GetIndex(name);
}

FString FNameTable::GetName(size_t index) const
{
    return mImpl->GetName(index).c_str();
}

bool FNameTable::IsValid(size_t index) const
{

    return mImpl->IsValid(index);
}

void FNameTable::Serialize(Arch &arch) {


    return mImpl->Serialize(arch);
}