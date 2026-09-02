#pragma once

#include "PhysicalFileSystem/PhysicalFileSystem.h"
#include "PhysicalFileSystem/PhysicalFileSystemDLLMacro.h"

#include <filesystem>
#include <string>
#include <vector>

namespace QuadPF
{

class PHYSICAL_FILESYSTEM_API StandardPhysicalFileSystem : public PhysicalFileSystem
{

  public:
    StandardPhysicalFileSystem();
    virtual ~StandardPhysicalFileSystem();

    // 향후에 더많은 매개변수를 통해 다양한 제어를 제공할수있다.

    bool CreatePhysicalFile(const std::filesystem::path &path) override;
    bool CreatePhysicalFolder(const std::filesystem::path &path) override;

    // 이 두 메서드는 하나로 합칠수도있을것이다.
    bool DeletePhysicalFile(const std::filesystem::path &path) override;

    // 하위 folder,file들을 모두제거
    bool DeletePhysicalFolder(const std::filesystem::path &path) override;

    bool ReadFileToBuffer(const std::filesystem::path &path, std::vector<uint8_t> &oBuffer) override;

    // 기존에있는 존재하는 파일에 쓰기
    bool WriteBufferToValidFile(const std::filesystem::path &path, const void *data, size_t size) override;

    bool WriteBufferToFile(const std::filesystem::path &path, const void *data, size_t size) override;

    // bool IsValidFolderPath(const std::string &path) const override;
    // bool IsValidFilePath(const std::string &path) const override;

    //// 유효한 파일,폴더 인지 확인할수있다.
    // bool IsValidPath(const std::string &path, bool &isFolder) const override;

    bool GetFileListByExtension(const std::filesystem::path &path, const std::string &extension,
                                std::vector<std::string> &oFileList) override;

    void GetFileList(const std::filesystem::path &path, std::vector<std::filesystem::path> &oFileList) const override;

  private:
    void LogError() const;

    bool WriteDataToFile(const std::filesystem::path &path, const void *data, size_t size);

  private:
};

} // namespace QuadPF
