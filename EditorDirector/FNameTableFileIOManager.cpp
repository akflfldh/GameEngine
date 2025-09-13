#include "FNameTableFileIOManager.h"
#include <CoreBase/FNameTable.h>
Quad::FNameTableFileIOManager *Quad::FNameTableFileIOManager::GetInstance()
{

    static FNameTableFileIOManager instance;
    return &instance;
}

Quad::FNameTableFileIOManager::FNameTableFileIOManager() : mNameTable(FNameTable::GetInstance()) {}

Quad::FNameTableFileIOManager::~FNameTableFileIOManager() {}
