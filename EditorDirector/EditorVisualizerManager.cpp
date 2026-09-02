#include "EditorVisualizerManager.h"
#include "CameraComponentVisualizer.h"
#include "LightComponentVisualizer.h"
#include "PlayerStartVisualizer.h"
EditorVisualizerManager::EditorVisualizerManager()
{

    mVisualizerList.push_back(std::make_unique<CameraComponentVisualizer>());
    mVisualizerList.push_back(std::make_unique<LightComponentVisualizer>());
    mVisualizerList.push_back(std::make_unique<PlayerStartVisualizer>());
}

EditorVisualizerManager::~EditorVisualizerManager() {}

void EditorVisualizerManager::Initialize(Map *editorMap)
{

    for (auto &pVisualizer : mVisualizerList)
    {
        pVisualizer->Initialize(editorMap);
    }
}
void EditorVisualizerManager::BindMap(Map *sourceMap)
{

    for (auto &pVisualizer : mVisualizerList)
    {
        pVisualizer->BindMap(sourceMap);
    }
}

void EditorVisualizerManager::Update(CameraComponent *editorCamera)
{

    for (auto &visualizer : mVisualizerList)
    {
        visualizer->Update(editorCamera);
    }


}