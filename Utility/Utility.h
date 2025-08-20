#pragma once

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

    //  AAA/SSS/DDD  , AAA/SSS/DDD.asd  - > AAA/SSS 리턴
    static std::string GetParentFolderPathFromPath(const std::string &path);

    static std::vector<std::string> Split(const std::string &str, char delimiter);

    // top > bottom (y축이 위로갈수록 증가)
    static bool IsPointInsideRect(float left, float right, float top, float bottom, float pointX, float pointY);

  private:
};

} // namespace CoreUtility