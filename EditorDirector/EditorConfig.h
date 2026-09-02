#pragma once

#include <filesystem>
#include <string>

enum class EEditorMode : uint8_t
{
    eProjectBrowser = 0,
    eEditProject
};

namespace Quad
{

class EditorConfig
{
  public:
    static EditorConfig *GetInstance();
    EditorConfig();
    ~EditorConfig();

    void SetEditorRootPath(const std::filesystem::path &path);
    const std::filesystem::path &GetEditorRootPath() const;

    // const std::string &GetEditorRootPath() const;

    void SetEditorExecutableDirectory(const std::filesystem::path &path);

    // 에디터  실행파일 위치
    const std::filesystem::path &GetExecutableDirectory() const;

    std::filesystem::path GetEditorAssetPath() const;

    void SetEditorMode(EEditorMode mode);
    EEditorMode GetEditorMode() const;

  private:
    // 배포버전에서는 ExecutablePath와동일 , 개발버전은 다른경로
    std::filesystem::path mEditorRootPath;
    // editor실행파일이 존재하는 경로(실행파일명은제외)
    std::filesystem::path mExecutablePath;

    std::filesystem::path mEditorAssetPath;

    EEditorMode mEditorMode;
};

} // namespace Quad