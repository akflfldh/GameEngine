#include "PlayerStartVisualizer.h"
#include "EditorPlayerStartVisualizerObject.h"
#include <Core/Map.h>
#include <algorithm>
PlayerStartVisualizer::PlayerStartVisualizer() {}

PlayerStartVisualizer::~PlayerStartVisualizer() {}

void PlayerStartVisualizer::OnBeforeBindingMap()
{

    Map *sourceMap = GetSourceMap();

    if (sourceMap)
    {
        sourceMap->mOnPlayerStartAddedCallbackSystem.UnRegister(mPlayerStartAddedCallbackID);
        sourceMap->mOnPlayerStartRemovedCallbackSystem.UnRegister(mPlayerStartRemovedCallbackID);
        ClearVisualObjects();
    }
}
void PlayerStartVisualizer::Update(CameraComponent *editorCamera) {}

void PlayerStartVisualizer::OnBindMap(Map *sourceMap)
{

    if (sourceMap)
    {

        mPlayerStartAddedCallbackID = sourceMap->mOnPlayerStartAddedCallbackSystem.Register(
            [this](PlayerStart *playerStart) { OnPlayerStartAdded(playerStart); });
        mPlayerStartRemovedCallbackID = sourceMap->mOnPlayerStartRemovedCallbackSystem.Register(
            [this](PlayerStart *playerStart) { OnPlayerStartRemoved(playerStart); });
    }
    RebuildVisualObjects();
}

void PlayerStartVisualizer::OnPlayerStartAdded(PlayerStart *playerStart)
{

    AddVisualObject(playerStart);
}

void PlayerStartVisualizer::OnPlayerStartRemoved(PlayerStart *playerStart)
{

    RemoveVisualObject(playerStart);
}

void PlayerStartVisualizer::RemoveVisualObject(PlayerStart *playerStart)
{

    Map *editorMap = GetEditorMap();
    if (editorMap == nullptr)
        return;

    Object *targetObject = nullptr;
    size_t targetIndex = -1;
    for (size_t i = 0; i < mEntityList.size(); ++i)
    {
        EditorPlayerStartVisualizerObject *visiualizerObject = mEntityList[i];

        if (visiualizerObject && visiualizerObject->GetSourcePlayerStart() == playerStart)
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

void PlayerStartVisualizer::AddVisualObject(PlayerStart *playerStart)
{

    Map *editorMap = GetEditorMap();
    if (editorMap == nullptr)
        return;

    EditorPlayerStartVisualizerObject *visualizerObject = static_cast<EditorPlayerStartVisualizerObject *>(
        editorMap->CreateEntity<EditorPlayerStartVisualizerObject>("PlayerStartVisualizerObject"));

    if (visualizerObject)
    {
        visualizerObject->Bind(playerStart);
        mEntityList.push_back(visualizerObject);
    }
}

void PlayerStartVisualizer::ClearVisualObjects()
{

    for (auto ob : mEntityList)
    {

        if (ob)
            ob->Destroy();
    }

    mEntityList.clear();
}

void PlayerStartVisualizer::RebuildVisualObjects()
{

    Map *sourceMap = GetSourceMap();
    Map *editorMap = GetEditorMap();

    if (sourceMap == nullptr || editorMap == nullptr)
        return;

    for (auto playerStart : sourceMap->GetPlayerStartList())
    {
        if (playerStart == nullptr)
            continue;

        auto object = editorMap->CreateEntity<EditorPlayerStartVisualizerObject>("PlayerStartVisualizer");

        if (object)
        {
            object->Bind(playerStart);
            mEntityList.push_back(object);
        }
    }
}