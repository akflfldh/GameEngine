
#include "Utility/NameTable.h"
#include"Windows.h"

bool Quad::NameTable::Register(const std::string& key, unsigned long long id)
{

    if (GetID(key) != 0)    //해당이름이 이미존재
        return false;

    mTable[key] = id;

    
    return true;
}

void Quad::NameTable::UnRegister(const std::string& key)
{
    mTable.erase(key);
    

}

unsigned long long Quad::NameTable::GetID(const std::string& key) const
{
    
    std::unordered_map<std::string, unsigned long long >::const_iterator itr = mTable.find(key);
  
    return itr !=mTable.cend() ? itr->second : 0;

}

int Quad::NameTable::GetElementNum() const
{
    return mTable.size();
}
