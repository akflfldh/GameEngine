#include "Utility/Utility.h"
#include "uuid.h"
#include <string.h>
#include <unicode/errorcode.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/ustring.h>
#include <vector>

#ifdef D3DX
#include <Windows.h>

#endif

std::string CoreUtility::Utility::ConvertToString(const std::wstring &wstr, bool unicode)
{
    icu::UnicodeString u16String = wstr.c_str();
    std::string str;
    u16String.toUTF8String(str);
    return str;
}

std::wstring CoreUtility::Utility::ConvertToWString(const std::string &str, bool unicode)
{
    icu::UnicodeString u16String = icu::UnicodeString::fromUTF8(str);

    if (sizeof(wchar_t) == 2)
    {
        std::wstring wstr;
        size_t length = u16String.length();
        wstr.resize(length);

        u16String.extract(0, u16String.length(), icu::Char16Ptr(wstr.data()), 0);

        return wstr;
    }
    else if (sizeof(wchar_t) == 4)
    {
        std::vector<UChar32> uchar32Buffer(u16String.length() + 1);
        icu::ErrorCode errorCode;
        int32_t length = u16String.toUTF32(uchar32Buffer.data(), (int32_t)uchar32Buffer.size(), errorCode);
        if (!U_SUCCESS(errorCode))
        {
            // log
            return L"";
        }

        std::wstring wstr(reinterpret_cast<wchar_t *>(uchar32Buffer.data()), length);

        return wstr;
    }

    return L"";
}

void CoreUtility::Utility::ConvertUpperCase(std::string &oStr)
{

    icu::UnicodeString icuString = icu::UnicodeString::fromUTF8(oStr);
    icuString.toUpper();

    icuString.toUTF8String(oStr);
}

void CoreUtility::Utility::ConvertLowerCase(std::string &oStr)
{
    icu::UnicodeString icuString = icu::UnicodeString::fromUTF8(oStr);
    icuString.toLower();

    icuString.toUTF8String(oStr);
}

std::string CoreUtility::Utility::GetFileNameFromPath(const std::string &path, bool removeExtension)
{

    size_t slashPos = path.find_last_of('/');
    size_t backSlashPos = path.find_last_of('\\');

    std::size_t pos = std::max(slashPos, backSlashPos);

    if (slashPos == std::string::npos)
    {
        pos = backSlashPos;
    }
    else if (backSlashPos == std::string::npos)
    {
        pos = slashPos;
    }

    std::string fileName = (pos == std::string::npos) ? path : path.substr(pos + 1);

    if (removeExtension)
    {
        size_t dotPos = fileName.find_last_of('.');
        if (dotPos != std::string::npos)
        {
            fileName = fileName.substr(0, dotPos);
        }
    }

    return fileName;
}

std::string CoreUtility::Utility::GetExtensionFromPath(const std::string &path)
{

    size_t dotPos = path.find_last_of('.');
    size_t backSlashPos = path.find_last_of('\\');
    size_t slashPos = path.find_last_of('/');

    if (dotPos == std::string::npos)
        return "";

    size_t commonSlashPos = backSlashPos;
    if (backSlashPos == std::string::npos)
    {
        commonSlashPos = slashPos;
    }

    if (dotPos < commonSlashPos)
    {
        // 올바르지못하다.
        return "";
    }

    return path.substr(dotPos + 1);
}

std::string CoreUtility::Utility::RemoveExtension(const std::string &path)
{

    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos)
        return path;

    return path.substr(0, dotPos);
}

std::string CoreUtility::Utility::GetParentFolderPathFromPath(const std::string &path)
{

    size_t slashPos = path.find_last_of('/');
    size_t backSlashPos = path.find_last_of('\\');

    std::size_t pos = std::max(slashPos, backSlashPos);

    if (slashPos == std::string::npos)
    {
        pos = backSlashPos;
    }
    else if (backSlashPos == std::string::npos)
    {
        pos = slashPos;
    }

    std::string fileName = (pos == std::string::npos) ? path : path.substr(0, pos);

    return fileName;
}

std::vector<std::string> CoreUtility::Utility::Split(const std::string &str, char delimiter)
{

    std::vector<std::string> tokenVector;

    std::stringstream ss(str);

    std::string token;
    while (std::getline(ss, token, delimiter))
    {
        // 빈토큰 /folder1//folder의경우 건너띈다.
        if (!token.empty())
        {
            tokenVector.push_back(std::move(token));
        }
    }

    return tokenVector;
}

std::string CoreUtility::Utility::MakeUniqueName(const std::string &baseName)
{

    static std::mt19937 generator(
        []()
        {
            std::random_device rd;
            std::array<int, std::mt19937::state_size> seed_data{};
            std::generate(seed_data.begin(), seed_data.end(), std::ref(rd));
            std::seed_seq seq(seed_data.begin(), seed_data.end());
            return std::mt19937(seq);
        }());

    static uuids::uuid_random_generator uuidGenerator(generator);

    uuids::uuid const id = uuidGenerator();
    std::string idStr = uuids::to_string(id);

    return baseName + "_" + idStr;
}

bool CoreUtility::Utility::IsPointInsideRect(float left, float right, float top, float bottom, float pointX,
                                             float pointY)
{

    if (pointX < left)
        return false;
    if (pointX > right)
        return false;

    if (pointY > top)
        return false;
    if (pointY < bottom)
        return false;

    return true;
}

CoreUtility::UniqueID CoreUtility::Utility::MakeUniqueID()
{
    static std::mt19937 generator(
        []()
        {
            std::random_device rd;
            std::array<int, std::mt19937::state_size> seed_data{};
            std::generate(seed_data.begin(), seed_data.end(), std::ref(rd));
            std::seed_seq seq(seed_data.begin(), seed_data.end());
            return std::mt19937(seq);
        }());
    uuids::uuid_random_generator uuidGenerator(generator);
    uuids::uuid newUUID = uuidGenerator();

    UniqueID uniqueID;

    std::memcpy(uniqueID.mUniqueID, (const void *)&newUUID, 16);

    return uniqueID;
}

std::vector<uint32_t> CoreUtility::Utility::GetUnicodeFromUTF8(const std::string &utf8)
{

    icu::UnicodeString icuString = icu::UnicodeString::fromUTF8(utf8);
    const char16_t *buffer = icuString.getBuffer();
    uint32_t len = icuString.length();

    int32_t i = 0;

    std::vector<uint32_t> vec;
    while (i < len)
    {
        int32_t codepoint;

        U16_NEXT(buffer, i, len, codepoint);

        // 잘못된 문자
        if (codepoint < 0)
            continue;

        vec.push_back(codepoint);
    }
    return vec;
}

void CoreUtility::Utility::Utility_GetCurrentDirectoryW(size_t size, wchar_t *oBuffer)
{

    GetCurrentDirectoryW(size, oBuffer);
}

bool CoreUtility::Utility::IsAllAlpha(const std::string &text)
{

    for (auto ch : text)
    {
        if (std::isalpha(ch) == 0)
        {
            return false;
        }
    }

    return true;
}

bool CoreUtility::Utility::CreateNewProcess(const std::string &executionPath, const std::string &cmdArguments,
                                            const std::string &currentDirectoryPath)
{

#ifdef D3DX

    std::string cmdLine = executionPath;
    if (!cmdArguments.empty())
    {
        //"EditorDirector "cmd1 cmd2 cmd3""
        cmdLine += " " + cmdArguments;
    }

    char buffer[1024];
    strcpy_s(buffer, sizeof(buffer), cmdLine.c_str());

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    LPCSTR currentDirectory = nullptr;
    if (currentDirectoryPath != "")
    {
        currentDirectory = currentDirectoryPath.c_str();
    }
    bool ret = CreateProcessA(nullptr, buffer, nullptr, nullptr, false, 0, nullptr, currentDirectory, &si, &pi);
    if (ret)
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }
    else
    {
        DWORD error = GetLastError();
    }

    return false;

#endif
}

void *CoreUtility::LibraryUtility::Load(const std::filesystem::path &path)
{

#ifdef D3DX

    HMODULE handle = LoadLibraryA(path.string().c_str());
    if (handle == nullptr)
    {
        // 에러
        MessageBox(nullptr, "dll error", "error", MB_OK);
        return nullptr;
    }
    return handle;

#endif
}

bool CoreUtility::Utility::TryParseFloat(const std::string &text, float &oValue)
{

    if (text.empty())
        return false;

    try
    {
        size_t processedLength = 0;
        oValue = std::stof(text, &processedLength);
        return text.size() == processedLength;
    }
    catch (...)
    {
        return false;
    }

    return true;
}
