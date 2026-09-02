#include "EditorConfig.h"
#include <Windows.h>

Quad::EditorConfig *Quad::EditorConfig::GetInstance()
{

    static EditorConfig instance;
    return &instance;
}

Quad::EditorConfig::EditorConfig() {}

Quad::EditorConfig::~EditorConfig() {}

void Quad::EditorConfig::SetEditorRootPath(const std::filesystem::path &path)
{

    mEditorRootPath = path;
}

const std::filesystem::path &Quad::EditorConfig::GetEditorRootPath() const
{

    return mEditorRootPath;
}

void Quad::EditorConfig::SetEditorExecutableDirectory(const std::filesystem::path &path)
{

    mExecutablePath = path;
}

const std::filesystem::path &Quad::EditorConfig::GetExecutableDirectory() const
{

    return mExecutablePath;

    std::wstring buffer(32768, L'\0');

#ifdef _WIN32

    DWORD length = GetModuleFileNameW(nullptr, buffer.data(), buffer.size());

    buffer.resize(length);
#endif

    return std::filesystem::path(buffer).parent_path();
}

std::filesystem::path Quad::EditorConfig::GetEditorAssetPath() const
{

    return mEditorRootPath / "Engine";

    // TODO: 여기에 return 문을 삽입합니다.
}

void Quad::EditorConfig::SetEditorMode(EEditorMode mode)
{

    mEditorMode = mode;
}

EEditorMode Quad::EditorConfig::GetEditorMode() const
{
    return mEditorMode;
}
