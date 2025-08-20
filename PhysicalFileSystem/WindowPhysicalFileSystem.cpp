
#include <Utility/Utility.h>

#ifdef _WINDOWS

#include "PhysicalFileSystem/WindowHandleRef.h"
#include "PhysicalFileSystem/WindowPhysicalFileSystem.h"
#include <filesystem>

// 일단은 단순하게 c++표준 file i/o api를 사용해 구현하자

QuadPF::WindowsPhysicalFileSystem::WindowsPhysicalFileSystem() {}

QuadPF::WindowsPhysicalFileSystem::~WindowsPhysicalFileSystem() {}

bool QuadPF::WindowsPhysicalFileSystem::CreatePhysicalFile(const std::string &fileName, const std::string &path)
{

    // 경로 검사

    bool ret = IsValidFolderPath(path);
    if (ret == false)
        return false;

    const std::wstring fileNameFullPath = CoreUtility::Utility::ConvertToWString(path + "/" + fileName, true);

    WindowHandleRef handle = CreateFileW(fileNameFullPath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                                         CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (handle.GetHandle() == INVALID_HANDLE_VALUE)
    {
        LogError();
        return false;
    }

    return true;
}

bool QuadPF::WindowsPhysicalFileSystem::CreatePhysicalFolder(const std::string &folderName, const std::string &path)
{

    bool ret = IsValidFolderPath(path);
    if (ret == false)
        return false;

    const std::wstring folderFullPath = CoreUtility::Utility::ConvertToWString(path + "/" + folderName, true);
    if (CreateDirectoryW(folderFullPath.c_str(), nullptr) == 0)
    {

        LogError();
        return false;
    }

    return true;
}

bool QuadPF::WindowsPhysicalFileSystem::DeletePhysicalFile(const std::string &path)
{

    // if (!IsValidFilePath(path))
    //{
    //	//log
    //	return false;
    // }

    const std::wstring pathW = CoreUtility::Utility::ConvertToWString(path, true);

    if (DeleteFileW(pathW.c_str()) == 0)
    {
        LogError();
        return false;
    }

    return true;
}

bool QuadPF::WindowsPhysicalFileSystem::DeletePhysicalFolder(const std::string &path)
{

    if (IsValidFolderPath(path) == false)
    {
        // log
        return false;
    }

    std::error_code error;
    uintmax_t deletedObjectNum = std::filesystem::remove_all(path, error);
    if (deletedObjectNum == -1)
    {
        // log
        std::string errorStr = error.message();
        return false;
    }

    // SHFileOperation()
    return true;
}

bool QuadPF::WindowsPhysicalFileSystem::ReadFileToBuffer(const std::string &path, std::vector<uint8_t> &oBuffer)
{

    // file handle을 먼저 가져오고
    WindowHandleRef handleRef = OpenFile(path);
    HANDLE handle = handleRef.GetHandle();
    if (CheckFileHandle(handle, "파일열기실패") == false)
    {
        return false;
    }

    LARGE_INTEGER fileSize;
    // 파일사이즈 확인
    if (GetFileSizeEx(handle, &fileSize) == 0)
    {
        LogError();
        return false;
    }

    /// 버퍼 확보
    oBuffer.resize(fileSize.QuadPart);

    // ReadFile()
    if (ReadFile(handle, (void *)(oBuffer.data()), oBuffer.size(), nullptr, NULL) != TRUE)
    {
        LogError();
        return false;
    }

    return true;
}

bool QuadPF::WindowsPhysicalFileSystem::WriteBufferToValidFile(const std::string &path, void *data, size_t size)
{

    if (data == nullptr)
        return false;

    WindowHandleRef handleRef = OpenFile(path);
    HANDLE handle = handleRef.GetHandle();
    if (CheckFileHandle(handle, "파일열기실패") == false)
    {
        return false;
    }

    DWORD writtenSize = 0;
    bool ret = WriteDataToFile(handle, data, size, writtenSize);

    if (ret == false || size != writtenSize)
    {
        // log
        return false;
    }

    return true;
}

bool QuadPF::WindowsPhysicalFileSystem::WriteBufferToFile(const std::string &path, void *data, size_t size)
{

    if (data == nullptr)
        return false;

    WindowHandleRef handleRef = OpenFile(path, CREATE_ALWAYS);
    HANDLE handle = handleRef.GetHandle();
    if (CheckFileHandle(handle, "파일생성실패") == false)
    {
        return false;
    }

    DWORD writtenSize = 0;
    bool ret = WriteDataToFile(handle, data, size, writtenSize);
    if (ret == false || writtenSize != size)
        return false;

    return true;
}

bool QuadPF::WindowsPhysicalFileSystem::IsValidFolderPath(const std::string &path) const
{
    bool isFolderPath = false;
    bool ret = IsValidPath(path, isFolderPath);

    if (ret == false || isFolderPath == false)
        return false;

    return true;
}

bool QuadPF::WindowsPhysicalFileSystem::IsValidFilePath(const std::string &path) const
{
    bool isFolderPath = false;
    bool ret = IsValidPath(path, isFolderPath);

    if (ret == false || isFolderPath == true)
        return false;

    return true;
}

bool QuadPF::WindowsPhysicalFileSystem::IsValidPath(const std::string &path, bool &isFolder) const
{
    const std::wstring pathW = CoreUtility::Utility::ConvertToWString(path, true);

    DWORD attribute = GetFileAttributesW(pathW.c_str());

    if (attribute == INVALID_FILE_ATTRIBUTES)
    {
        LogError();
        return false;
    }

    if (attribute & FILE_ATTRIBUTE_DIRECTORY)
    {
        isFolder = true;
    }
    else
        isFolder = false;

    // 더정확히 파일경로인지, 폴더경로인지도 나타내주자.

    return true;
}

bool QuadPF::WindowsPhysicalFileSystem::GetFileListByExtension(const std::string &folderPath,
                                                               const std::string &extension,
                                                               std::vector<std::string> &oFileList)
{

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind;

    std::string path = folderPath;
    if (extension != "")
    {
        path += "/*." + extension;
    }
    else
    {
        path += "/*";
    }

    hFind = FindFirstFileW(CoreUtility::Utility::ConvertToWString(path, true).c_str(), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        // log
        LogError();
        return false;
    }

    do
    {
        oFileList.push_back(std::move(CoreUtility::Utility::ConvertToString(findFileData.cFileName, true)));

    } while (FindNextFileW(hFind, &findFileData));

    FindClose(hFind);

    return true;
}

void QuadPF::WindowsPhysicalFileSystem::LogError() const
{

    wchar_t *outputErrorBuffer = nullptr;

    // log
    DWORD errorStrNum =
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       nullptr, GetLastError(), 0, (LPWSTR)(&outputErrorBuffer), 0, nullptr);

    if (errorStrNum == 0)
    {
        DWORD error = GetLastError();
        // log error 값만 출력하자
    }
    else
    {
        // log에 출력
        std::wstring errorStrW(outputErrorBuffer, errorStrNum);

        std::string errorStr = CoreUtility::Utility::ConvertToString(errorStrW, true);
        // log
    }
}

HANDLE QuadPF::WindowsPhysicalFileSystem::OpenFile(const std::string &path, DWORD creationFlag)
{

    const std::wstring filePathW = CoreUtility::Utility::ConvertToWString(path, true);

    HANDLE handle = CreateFileW(filePathW.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, creationFlag,
                                FILE_ATTRIBUTE_NORMAL, NULL);

    return handle;
}

bool QuadPF::WindowsPhysicalFileSystem::CheckFileHandle(HANDLE handle, const std::string &logMessage)
{

    if (handle == INVALID_HANDLE_VALUE)
    {
        // logMessage
        return false;
    }

    return true;
}

bool QuadPF::WindowsPhysicalFileSystem::WriteDataToFile(HANDLE handle, void *data, size_t size, DWORD &writtenSize)
{
    if (WriteFile(handle, data, size, &writtenSize, NULL) == 0)
    {
        LogError();
        return false;
    }
    return true;
}

#endif