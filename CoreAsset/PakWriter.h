#pragma once

#include <CoreAsset/CoreAssetDLLMacro.h>
#include <CoreAsset/PakType.h>
#include <filesystem>
class CORE_ASSET_API PakWriter
{
  public:
    PakWriter();
    ~PakWriter();

    bool StorePak(const std::filesystem::path &outputPakPath,
                  const std::vector<PakWriteAssetRecord> &pakWriteAssetRecord);

  private:
    uint64_t GetHeaderSize() const;
    uint64_t GetEntrySize() const;
};
