#pragma once

#ifdef _WINDOWS

#include "PhysicalFileSystem/PhysicalFileSystem.h"
#include "PhysicalFileSystem/PhysicalFileSystemDLLMacro.h"
#include <Windows.h>
#include <string>
#include <vector>

namespace QuadPF
{

class PHYSICAL_FILESYSTEM_API WindowsPhysicalFileSystem : public PhysicalFileSystem
{

  public:
    WindowsPhysicalFileSystem();
    virtual ~WindowsPhysicalFileSystem();

    // 향후에 더많은 매개변수를 통해 다양한 제어를 제공할수있다.

    bool CreatePhysicalFile(const std::string &fileName, const std::string &path = ""
                            ) override;
    bool CreatePhysicalFolder(const std::string &folderName, const std::string &path) override;

    // 이 두 메서드는 하나로 합칠수도있을것이다.
    bool DeletePhysicalFile(const std::string &path) override;

    // 하위 folder,file들을 모두제거
    bool DeletePhysicalFolder(const std::string &path) override;

    bool ReadFileToBuffer(const std::string &path, std::vector<uint8_t> &oBuffer) override;

    // 기존에있는 존재하는 파일에 쓰기
    bool WriteBufferToValidFile(const std::string &path, void *data, size_t size) override;

    bool WriteBufferToFile(const std::string &path, void *data, size_t size) override;

    bool IsValidFolderPath(const std::string &path) const override;
    bool IsValidFilePath(const std::string &path) const override;

    // 유효한 파일,폴더 인지 확인할수있다.
    bool IsValidPath(const std::string &path, bool &isFolder) const override;

    bool GetFileListByExtension(const std::string &folderPath, const std::string &extension,
                                std::vector<std::string> &oFileList) override;

  private:
    void LogError() const;

    HANDLE OpenFile(const std::string &path, DWORD creationFlag = OPEN_EXISTING);

    // handle이 유효하면 true,  유효하지않으면 false , logMessage 로그를 남긴다.
    inline bool CheckFileHandle(HANDLE handle, const std::string &logMessage);

    bool WriteDataToFile(HANDLE handle, void *data, size_t size, DWORD &writtenSize);

  private:
};

} // namespace QuadPF

#endif