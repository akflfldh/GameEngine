#pragma once

#include <Utility/UniqueID.h>
#include <filesystem>
#include <string>
#include <vector>
namespace CoreUtility
{

class Utility
{
  public:
    static std::string ConvertToString(const std::wstring &wstr, bool unicode);

    static std::wstring ConvertToWString(const std::string &str, bool unicode);

    static void ConvertUpperCase(std::string &oStr);
    static void ConvertLowerCase(std::string &oStr);

    static std::string MakeUniqueName(const std::string &baseName); // 출력 : baseName_~~~~

    static std::string GetFileNameFromPath(const std::string &path, bool removeExtension = true);
    static std::string GetExtensionFromPath(const std::string &path);

    static std::string RemoveExtension(const std::string &path);

    //  AAA/SSS/DDD  , AAA/SSS/DDD.asd  - > AAA/SSS 리턴
    static std::string GetParentFolderPathFromPath(const std::string &path);

    static std::vector<std::string> Split(const std::string &str, char delimiter);

    // top > bottom (y축이 위로갈수록 증가)
    static bool IsPointInsideRect(float left, float right, float top, float bottom, float pointX, float pointY);

    static UniqueID MakeUniqueID();

    // UTF8
    static std::vector<uint32_t> GetUnicodeFromUTF8(const std::string &utf8);

    static void Utility_GetCurrentDirectoryW(size_t size, wchar_t *oBuffer);

    static bool IsAllAlpha(const std::string &text);

    static bool CreateNewProcess(const std::string &executionPath, const std::string &cmdArguments,
                                 const std::string &currentDirectoryPath = "");

    static bool TryParseFloat(const std::string &text, float &oValue);

  private:
};

class LibraryUtility
{
  public:
    static void *Load(const std::filesystem::path &path);
};

// FNV-1a
template <typename T> size_t HashValue(uint64_t hash, const T &value)
{

    const uint8_t *byte = reinterpret_cast<const uint8_t *>(&value);
    const uint64_t prime = 0x00000100000001b3;
    for (size_t i = 0; i < sizeof(value); ++i)
    {
        hash ^= byte[i];
        hash *= prime;
    }

    return hash;
}

} // namespace CoreUtility