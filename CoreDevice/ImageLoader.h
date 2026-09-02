#pragma once

#include <CoreDevice/CoreDeviceDllMacro.h>
#include <CoreDevice/CoreDeviceHeader.h>
#include <filesystem>

namespace Core
{
struct CoreTextureData
{
};

class CORE_DEVICE_API ImageLoader
{
  public:
    ~ImageLoader();

    static GRM::ScratchImage LoadFromMemory(const void *data, size_t size);
    static GRM::ScratchImage LoadFromFile(const std::filesystem::path &filePath);

  private:
    ImageLoader() = default;
};

} // namespace Core