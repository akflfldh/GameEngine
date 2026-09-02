#include "LightComponentVisualizer.h"
#include "EditorVisualizerManager.h"
#include "IEditorVisualizer.h"
#include <Core/Map.h>
#include <EditorDebugDraw.h>
#include <EditorLightVisualizerObject.h>
#include <algorithm>

void LightComponentVisualizer::Update(CameraComponent *editorCamera)
{

    for (auto object : mEntityList)
    {
        object->SyncFromSource();

        EditorDebugDraw ::DrawLightVisual(GetEditorMap(), object->GetSourceLight(), editorCamera);
    }
}
void LightComponentVisualizer::SetVisible(bool visible) {}

void LightComponentVisualizer::OnBeforeBindingMap()
{
    Map *sourceMap = GetSourceMap();

    if (sourceMap)
    {
        sourceMap->mOnLightComponentAddeddCallbackSystem.UnRegister(mLightComponentAddedCallbackID);
        sourceMap->mOnLightComponentRemovedCallbackSystem.UnRegister(mLightComponentRemovedCallbackID);
        ClearVisualObjects();
    }
}

void LightComponentVisualizer::OnBindMap(Map *sourceMap)
{

    if (sourceMap)
    {
        mLightComponentAddedCallbackID = sourceMap->mOnLightComponentAddeddCallbackSystem.Register(
            [this](LightComponent *com) { OnLightComponentAdded(com); });
        mLightComponentRemovedCallbackID = sourceMap->mOnLightComponentRemovedCallbackSystem.Register(
            [this](LightComponent *com) { OnLightComponentRemoved(com); });
    }
    RebuildVisualObject();
}

void LightComponentVisualizer::UnBindMap() {}

void LightComponentVisualizer::RebuildVisualObject()
{

    Map *sourceMap = GetSourceMap();
    Map *editorMap = GetEditorMap();

    if (sourceMap == nullptr || editorMap == nullptr)
        return;

    for (auto com : sourceMap->GetLightComponentList())
    {
        if (com == nullptr)
            continue;

        auto object = editorMap->CreateEntity<EditorLightVisualizerObject>("LightVisualizer");

        if (object)
        {
            object->Bind(com);
            mEntityList.push_back(object);
        }
    }
}

void LightComponentVisualizer::OnLightComponentAdded(LightComponent *com)
{
    AddVisualObject(com);
}

void LightComponentVisualizer::OnLightComponentRemoved(LightComponent *com)
{
    RemoveVisualObject(com);
}

void LightComponentVisualizer::AddVisualObject(LightComponent *com)
{

    Map *editorMap = GetEditorMap();

    if (editorMap == nullptr)
        return;

    auto object = editorMap->CreateEntity<EditorLightVisualizerObject>("LightVisualizer");

    if (object)
    {
        object->Bind(com);
        mEntityList.push_back(object);
    }
}

void LightComponentVisualizer::RemoveVisualObject(LightComponent *com)
{
    if (com == nullptr)
        return;

    Object *targetObject = nullptr;
    size_t targetIndex = -1;
    for (size_t i = 0; i < mEntityList.size(); ++i)
    {
        EditorLightVisualizerObject *visiualizerObject = mEntityList[i];

        if (visiualizerObject && visiualizerObject->GetSourceLight() == com)
        {
            targetObject = visiualizerObject;
            targetIndex = i;
        }
    }

    if (targetIndex == -1)
        return;

    std::iter_swap(mEntityList.end() - 1, mEntityList.begin() + targetIndex);
    mEntityList.pop_back();
    targetObject->Destroy();
}

void LightComponentVisualizer::ClearVisualObjects()
{

    for (auto ob : mEntityList)

    {
        if (ob)
            ob->Destroy();
    }

    mEntityList.clear();
}