#pragma once
#pragma once

#include <Core/CoreDllExport.h>
#include <CoreAsset/AssetMetaDataType.h>

class Arch;

namespace Core
{
struct CORE_API_LIB PrefabMetaData : public CoreAsset::AssetMetaData
{
};

struct CORE_API_LIB PrefabCommonHeader
{

    int i = 0;

    void Serialize(Arch &arch);
};

} // namespace Core