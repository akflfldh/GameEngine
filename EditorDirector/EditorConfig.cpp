#include "EditorConfig.h"

Quad::EditorConfig *Quad::EditorConfig::GetInstance()
{

    static EditorConfig instance;
    return &instance;
}

Quad::EditorConfig::EditorConfig() {}

Quad::EditorConfig::~EditorConfig() {}

void Quad::EditorConfig::SetEditorRootPath(const std::string &editorRootPath)
{

    mEditorRootPath = editorRootPath;
}

const std::string &Quad::EditorConfig::GetEditorRootPath() const
{

    return mEditorRootPath;

    // TODO: 여기에 return 문을 삽입합니다.
}

std::string Quad::EditorConfig::GetEditorAssetPath() const
{

    return mEditorRootPath + "/Asset";

    // TODO: 여기에 return 문을 삽입합니다.
}
