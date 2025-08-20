#pragma once

#include <string>

namespace Quad
{

class EditorConfig
{
  public:
    static EditorConfig *GetInstance();
    EditorConfig();
    ~EditorConfig();

    void SetEditorRootPath(const std::string &editorRootPath);
    const std::string &GetEditorRootPath() const;

    std::string GetEditorAssetPath() const;


  private:
    // editor실행파일이 존재하는 경로(실행파일명은제외)
    std::string mEditorRootPath;
};

} // namespace Quad