#include "IEditorVisualizer.h"

IEditorVisualizer::IEditorVisualizer() {}

IEditorVisualizer::~IEditorVisualizer() {}

void IEditorVisualizer::Initialize(Map *editorMap)
{

    mEditorMap = editorMap;
}
void IEditorVisualizer::BindMap(Map *sourceMap)
{
    OnBeforeBindingMap();
    mSourceMap = sourceMap;
    OnBindMap(sourceMap);
}

void IEditorVisualizer::UnBindMap() {}

Map *IEditorVisualizer::GetEditorMap() const
{

    return mEditorMap;
}
Map *IEditorVisualizer::GetSourceMap() const
{

    return mSourceMap;
}

void IEditorVisualizer::OnBeforeBindingMap() {}

void IEditorVisualizer::OnBindMap(Map *sourceMap) {}
