#pragma once

// #include < string>
#include <unordered_map>
#include <vector>

#include <CoreBase/FString.h>
#include <mutex>
class FNameTableImpl
{

  public:
    static FNameTableImpl *GetInstance();
    FNameTableImpl();
    ~FNameTableImpl();

    size_t GetIndex(const FString &name) const;
    FString GetName(size_t index) const;

    bool IsValid(size_t index) const;

    void Serialize(Arch &arch);

  private:
    size_t RegisterName(const FString &name) const;

  private:
    mutable std::unordered_map<FString, size_t> mNameIndexTable;
    mutable std::vector<FString> mNameList;

    mutable std::mutex mMutex;
};
