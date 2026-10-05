#include "EditorVisualizerManager.h"
#include "CameraComponentVisualizer.h"
#include "BoxColliderComponentVisualizer.h"
#include "LightComponentVisualizer.h"
#include "PlayerStartVisualizer.h"
EditorVisualizerManager::EditorVisualizerManager()
{

    mVisualizerList.push_back(std::make_unique<CameraComponentVisualizer>());
    mVisualizerList.push_back(std::make_unique<LightComponentVisualizer>());
    mVisualizerList.push_back(std::make_unique<PlayerStartVisualizer>());
    mVisualizerList.push_back(std::make_unique<BoxColliderComponentVisualizer>());
}

EditorVisualizerManager::~EditorVisualizerManager() {}

void EditorVisualizerManager::Initialize(Map *editorMap)
{

    for (auto &pVisualizer : mVisualizerList)
    {
        pVisualizer->Initialize(editorMap);
    }
}
void EditorVisualizerManager::BindMap(Map *sourceMap, bool bUseLightVisualizer)
{

    for (auto &pVisualizer : mVisualizerList)
    {
        // 조명 시각화를 사용하지 않는 미리보기에서는 콜백과 빌보드를 만들지 않는다.
        // nullptr 재바인딩은 기존 연결도 정리하므로 이전 맵의 조명 방향선이 계속 제출되지 않는다.
        if (!bUseLightVisualizer && dynamic_cast<LightComponentVisualizer *>(pVisualizer.get()))
            pVisualizer->BindMap(nullptr);
        else
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
