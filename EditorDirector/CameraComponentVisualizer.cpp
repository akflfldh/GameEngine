#include "CameraComponentVisualizer.h"
#include <Core/DebugDraw.h>
#include <Core/Map.h>
#include <EditorCameraVisualizerObject.h>
#include <EditorDebugDraw.h>
#include <algorithm>

CameraComponentVisualizer::CameraComponentVisualizer() {}

CameraComponentVisualizer::~CameraComponentVisualizer() {}

void CameraComponentVisualizer::Update(CameraComponent *editorCamera)
{

    //
    for (auto object : mEntityList)
    {
        if (object)
        {
            object->SyncFromSource();
            DebugDraw::DrawLine(object->GetMap(), object->GetPositionWorld(), object->GetForwardWorld(), 100.0f,
                                {1, 0, 0, 1});
        }
    }
}
void CameraComponentVisualizer::SetVisible(bool visible) {}

void CameraComponentVisualizer::OnBeforeBindingMap()
{
    Map *sourceMap = GetSourceMap();

    if (sourceMap)
    {
        sourceMap->mOnCameraComponentAddedCallbackSystem.UnRegister(mCameraComponentAddedCallbackID);
        sourceMap->mOnCameraComponentRemovedCallbackSystem.UnRegister(mCameraComponentRemovedCallbackID);
        ClearVisualObjects();
    }
}

void CameraComponentVisualizer::OnBindMap(Map *sourceMap)
{

    if (sourceMap)
    {
        mCameraComponentAddedCallbackID = sourceMap->mOnCameraComponentAddedCallbackSystem.Register(
            [this](CameraComponent *com) { OnCameraComponentAdded(com); });
        mCameraComponentRemovedCallbackID = sourceMap->mOnCameraComponentRemovedCallbackSystem.Register(
            [this](CameraComponent *com) { OnCameraComponentRemoved(com); });
    }

    RebuildVisualObjects();
}

void CameraComponentVisualizer::RebuildVisualObjects()
{

    Map *sourceMap = GetSourceMap();
    Map *editorMap = GetEditorMap();

    if (sourceMap == nullptr || editorMap == nullptr)
        return;

    for (auto cameraCom : sourceMap->GetCameraComList())
    {
        if (cameraCom == nullptr)
            continue;

        auto object = editorMap->CreateEntity<EditorCameraVisualizerObject>("CameraVisualizer");

        if (object)
        {
            object->Bind(cameraCom);
            mEntityList.push_back(object);
        }
    }
}

void CameraComponentVisualizer::OnCameraComponentAdded(CameraComponent *com)
{

    AddVisualObject(com);
}
void CameraComponentVisualizer::OnCameraComponentRemoved(CameraComponent *com)
{

    RemoveVisualObject(com);
}

void CameraComponentVisualizer::RemoveVisualObject(CameraComponent *com)
{

    if (com == nullptr)
        return;

    Object *targetObject = nullptr;
    size_t targetIndex = -1;
    for (size_t i = 0; i < mEntityList.size(); ++i)
    {
        EditorCameraVisualizerObject *visiualizerObject = mEntityList[i];

        if (visiualizerObject && visiualizerObject->GetSourceCamera() == com)
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

void CameraComponentVisualizer::AddVisualObject(CameraComponent *com)
{
    Map *editorMap = GetEditorMap();

    auto object = editorMap->CreateEntity<EditorCameraVisualizerObject>("CameraVisualizer");

    if (object)
    {
        object->Bind(com);
        mEntityList.push_back(object);
    }
}

void CameraComponentVisualizer::ClearVisualObjects()
{

    for (auto ob : mEntityList)
    {
        if (ob)
            ob->Destroy();
    }

    mEntityList.clear();
}
