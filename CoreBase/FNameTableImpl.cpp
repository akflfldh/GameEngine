#include "FNameTableImpl.h"
#include <CoreBase/Arch.h>
FNameTableImpl *FNameTableImpl::GetInstance()
{

    static FNameTableImpl instance;

    return &instance;
}

FNameTableImpl::FNameTableImpl() {}

FNameTableImpl::~FNameTableImpl() {}

size_t FNameTableImpl::GetIndex(const FString &name) const
{

    std::lock_guard lock(mMutex);

    std::unordered_map<FString, size_t>::const_iterator it = mNameIndexTable.find(name);
    if (it == mNameIndexTable.cend())
    {
        // 새롭게 추가
        size_t index = RegisterName(name);
        return index;
    }

    return it->second;
}

FString FNameTableImpl::GetName(size_t index) const
{
    std::lock_guard<std::mutex> lock(mMutex);

    return mNameList[index];
}

bool FNameTableImpl::IsValid(size_t index) const
{
    if (mNameList.size() <= index)
        return false;

    return true;
}

void FNameTableImpl::Serialize(Arch &arch)
{
    if (arch.GetLoadingFlag())
    {
        while (!arch.IsEof())
        {
            // arch의 상태에 eof가  설정될떄까지
            FString name;
            arch << name;
            RegisterName(name);
        }
    }
    else
    {
        for (auto &name : mNameList)
        {
            arch << name;
        }
    }
}

size_t FNameTableImpl::RegisterName(const FString &name) const
{
    size_t index = mNameList.size();
    mNameList.push_back(name);
    mNameIndexTable[name] = index;

    return index;
}
