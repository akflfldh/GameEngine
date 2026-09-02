
#include "PhysicalFileSystem/StandardPhysicalFileSystem.h"
// #include "PhysicalFileSystem/WindowHandleRef.h"
#include <Logger/Logger.h>
#include <Utility/Utility.h>
#include <filesystem>
#include <fstream>

// 일단은 단순하게 c++표준 file i/o api를 사용해 구현하자

QuadPF::StandardPhysicalFileSystem::StandardPhysicalFileSystem() {}

QuadPF::StandardPhysicalFileSystem::~StandardPhysicalFileSystem() {}

bool QuadPF::StandardPhysicalFileSystem::CreatePhysicalFile(const std::filesystem::path &path)
{

    std::error_code errorCode;

    if (std::filesystem::exists(path, errorCode))
        return false;

    if (std::filesystem::is_directory(path, errorCode))
        return false;

    if (std::filesystem::is_regular_file(path, errorCode))
    {
        std::ofstream fout(path);

        if (!fout.is_open())
        {

            // LogError();
            return false;
        }
    }
    else
    {
        return false;
    }

    //// 경로 검사
    // std::string fullPath;
    // if (path != "")
    //{
    //     bool ret = IsValidFolderPath(path);
    //     if (ret == false)
    //         return false;

    //    fullPath = path + "/" + fileName;
    //}
    // else
    //{
    //    fullPath = fileName;
    //}

    // const std::wstring fileNameFullPath = CoreUtility::Utility::ConvertToWString(fullPath, true);

    // WindowHandleRef handle = CreateFileW(fileNameFullPath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
    //                                      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // if (handle.GetHandle() == INVALID_HANDLE_VALUE)
    //{
    //     LogError();
    //     return false;
    // }

    return true;
}

bool QuadPF::StandardPhysicalFileSystem::CreatePhysicalFolder(const std::filesystem::path &path)
{
    if (std::filesystem::is_directory(path))
    {
        std::error_code errorcode;
        return std::filesystem::create_directory(path, errorcode);
    }
    else
    {
        return false;
    }

    /* bool ret = IsValidFolderPath(path);
     if (ret == false)
         return false;

     const std::wstring folderFullPath = CoreUtility::Utility::ConvertToWString(path + "/" + folderName, true);
     if (CreateDirectoryW(folderFullPath.c_str(), nullptr) == 0)
     {

         LogError();
         return false;
     }

     return true;*/
}

bool QuadPF::StandardPhysicalFileSystem::DeletePhysicalFile(const std::filesystem::path &path)
{

    if (!std::filesystem::is_regular_file(path))
    {
        return false;
    }

    std::error_code errorcode;
    return std::filesystem::remove(path, errorcode);

    //// if (!IsValidFilePath(path))
    ////{
    ////	//log
    ////	return false;
    //// }

    // const std::wstring pathW = CoreUtility::Utility::ConvertToWString(path, true);

    // if (DeleteFileW(pathW.c_str()) == 0)
    //{
    //     LogError();
    //     return false;
    // }

    // return true;
}

bool QuadPF::StandardPhysicalFileSystem::DeletePhysicalFolder(const std::filesystem::path &path)
{

    if (!std::filesystem::is_directory(path))
    {

        return false;
    }

    std::error_code errorcode;
    return std::filesystem::remove(path, errorcode);

    // if (IsValidFolderPath(path) == false)
    //{
    //     // log
    //     return false;
    // }

    // std::error_code error;
    // uintmax_t deletedObjectNum = std::filesystem::remove_all(path, error);
    // if (deletedObjectNum == -1)
    //{
    //     // log
    //     std::string errorStr = error.message();
    //     return false;
    // }

    //// SHFileOperation()
    // return true;
}

bool QuadPF::StandardPhysicalFileSystem::ReadFileToBuffer(const std::filesystem::path &path,
                                                          std::vector<uint8_t> &oBuffer)
{

    if (!std::filesystem::is_regular_file(path))
    {
        return false;
    }

    std::ifstream fin(path.c_str(), std::ios_base::binary);

    if (!fin.is_open())
    {
        return false;
    }

    fin.seekg(0, std::ios_base::end);
    std::streampos fileSize = fin.tellg();

    if (fileSize < 0)
        return false;

    fin.seekg(0, std::ios_base::beg);

    std::vector<uint8_t> tempBuffer(fileSize);

    fin.read((char *)tempBuffer.data(), fileSize);

    if (!fin || fin.gcount() != fileSize)
        return false;

    oBuffer = std::move(tempBuffer);

    return true;

    //// file handle을 먼저 가져오고
    // WindowHandleRef handleRef = OpenFile(path);
    // HANDLE handle = handleRef.GetHandle();
    // if (CheckFileHandle(handle, "파일열기실패") == false)
    //{
    //     LOG_MESSAGE_CRITICAL("File", "파일열기실패");
    //     return false;
    // }

    // LARGE_INTEGER fileSize;
    //// 파일사이즈 확인
    // if (GetFileSizeEx(handle, &fileSize) == 0)
    //{
    //     LogError();
    //     return false;
    // }

    ///// 버퍼 확보
    // oBuffer.resize(fileSize.QuadPart);

    //// ReadFile()
    // if (ReadFile(handle, (void *)(oBuffer.data()), oBuffer.size(), nullptr, NULL) != TRUE)
    //{
    //     LogError();
    //     return false;
    // }

    // return true;
}

bool QuadPF::StandardPhysicalFileSystem::WriteBufferToValidFile(const std::filesystem::path &path, const void *data,
                                                                size_t size)
{
    if (data == nullptr)
    {
        return false;
    }

    std::error_code errorcode;
    if (!std::filesystem::is_regular_file(path, errorcode))
    {
        return false;
    }

    return WriteDataToFile(path, data, size);

    // if (data == nullptr)
    //     return false;

    // WindowHandleRef handleRef = OpenFile(path);
    // HANDLE handle = handleRef.GetHandle();
    // if (CheckFileHandle(handle, "파일열기실패") == false)
    //{
    //     return false;
    // }

    // DWORD writtenSize = 0;
    // bool ret = WriteDataToFile(handle, data, size, writtenSize);

    // if (ret == false || size != writtenSize)
    //{
    //     // log
    //     return false;
    // }

    // return true;
}

bool QuadPF::StandardPhysicalFileSystem::WriteBufferToFile(const std::filesystem::path &path, const void *data,
                                                           size_t size)
{

    return WriteDataToFile(path, data, size);

    // if (data == nullptr || size > 0)
    //     return false;

    // std::ofstream fout(path, std::ios_base::binary | std::ios_base::trunc);
    // if (!fout.is_open())
    //{
    //     return false;
    // }

    // if (size > 0)
    //     fout.write((char *)data, size);

    // if (!fout)
    //     return false;

    // return true;

    /*  WindowHandleRef handleRef = OpenFile(path, CREATE_ALWAYS);
      HANDLE handle = handleRef.GetHandle();
      if (CheckFileHandle(handle, "파일생성실패") == false)
      {
          return false;
      }

      DWORD writtenSize = 0;
      bool ret = WriteDataToFile(handle, data, size, writtenSize);
      if (ret == false || writtenSize != size)
          return false;

      return true;*/
}

// bool QuadPF::StandardPhysicalFileSystem::IsValidFolderPath(const std::string &path) const
//{
//     bool isFolderPath = false;
//     bool ret = IsValidPath(path, isFolderPath);
//
//     if (ret == false || isFolderPath == false)
//         return false;
//
//     return true;
// }
//
// bool QuadPF::StandardPhysicalFileSystem::IsValidFilePath(const std::string &path) const
//{
//     bool isFolderPath = false;
//     bool ret = IsValidPath(path, isFolderPath);
//
//     if (ret == false || isFolderPath == true)
//         return false;
//
//     return true;
// }
//
// bool QuadPF::StandardPhysicalFileSystem::IsValidPath(const std::string &path, bool &isFolder) const
//{
//     const std::wstring pathW = CoreUtility::Utility::ConvertToWString(path, true);
//
//     DWORD attribute = GetFileAttributesW(pathW.c_str());
//
//     if (attribute == INVALID_FILE_ATTRIBUTES)
//     {
//         LogError();
//         return false;
//     }
//
//     if (attribute & FILE_ATTRIBUTE_DIRECTORY)
//     {
//         isFolder = true;
//     }
//     else
//         isFolder = false;
//
//     // 더정확히 파일경로인지, 폴더경로인지도 나타내주자.
//
//     return true;
// }

bool QuadPF::StandardPhysicalFileSystem::GetFileListByExtension(const std::filesystem::path &path,
                                                                const std::string &extension,
                                                                std::vector<std::string> &oFileList)
{

    std::error_code errorcode;

    if (!std::filesystem::is_directory(path, errorcode))
    {
        return false;
    }

    std::string dotExtension = "." + extension;

    std::filesystem::directory_iterator it(path);

    for (const auto &entry : it)
    {

        if (entry.is_regular_file() && entry.path().extension() == dotExtension)
        {
            oFileList.push_back(entry.path().filename().string());
        }
    }

    // WIN32_FIND_DATAW findFileData;
    // HANDLE hFind;

    // std::string path = folderPath;
    // if (extension != "")
    //{
    //     path += "/*." + extension;
    // }
    // else
    //{
    //     path += "/*";
    // }

    // hFind = FindFirstFileW(CoreUtility::Utility::ConvertToWString(path, true).c_str(), &findFileData);
    // if (hFind == INVALID_HANDLE_VALUE)
    //{
    //     // log
    //     LogError();
    //     return false;
    // }

    // do
    //{
    //     oFileList.push_back(std::move(CoreUtility::Utility::ConvertToString(findFileData.cFileName, true)));

    //} while (FindNextFileW(hFind, &findFileData));

    // FindClose(hFind);

    // return true;
}

void QuadPF::StandardPhysicalFileSystem::GetFileList(const std::filesystem::path &path,
                                                     std::vector<std::filesystem::path> &oFileList) const
{
    if (!std::filesystem::is_directory(path))
        return;

    auto it = std::filesystem::directory_iterator(path);
    for (const auto &entry : it)
    {

        if (entry.is_regular_file())
        {
            oFileList.push_back(entry.path());
        }
    }
}

// void QuadPF::StandardPhysicalFileSystem::LogError() const
//{
//
//     wchar_t *outputErrorBuffer = nullptr;
//
//     // log
//     DWORD errorStrNum =
//         FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//                        nullptr, GetLastError(), 0, (LPWSTR)(&outputErrorBuffer), 0, nullptr);
//
//     if (errorStrNum == 0)
//     {
//         DWORD error = GetLastError();
//         // log error 값만 출력하자
//         LOG_MESSAGE_CRITICAL("File", " 파일에러 errorStrNum == 0 ");
//     }
//     else
//     {
//         // log에 출력
//         std::wstring errorStrW(outputErrorBuffer, errorStrNum);
//
//         std::string errorStr = CoreUtility::Utility::ConvertToString(errorStrW, true);
//         // log
//
//         LOG_MESSAGE_CRITICAL("File", errorStr.c_str());
//     }
// }
//
//  HANDLE QuadPF::StandardPhysicalFileSystem::OpenFile(const std::string &path, DWORD creationFlag)
//{
//
//     const std::wstring filePathW = CoreUtility::Utility::ConvertToWString(path, true);
//
//     HANDLE handle = CreateFileW(filePathW.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, creationFlag,
//                                 FILE_ATTRIBUTE_NORMAL, NULL);
//
//     return handle;
// }
//
//  bool QuadPF::StandardPhysicalFileSystem::CheckFileHandle(HANDLE handle, const std::string &logMessage)
//{
//
//     if (handle == INVALID_HANDLE_VALUE)
//     {
//         // logMessage
//         return false;
//     }
//
//     return true;
// }

bool QuadPF::StandardPhysicalFileSystem::WriteDataToFile(const std::filesystem::path &path, const void *data,
                                                         size_t size)
{

    if (data == nullptr && size > 0)
        return false;

    std::ofstream fout(path, std::ios_base::binary | std::ios_base::trunc);
    if (!fout.is_open())
    {
        return false;
    }

    if (size > 0)
        fout.write((char *)data, size);

    if (!fout)
        return false;

    return true;
}
