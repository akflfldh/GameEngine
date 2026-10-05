#include "BoxColliderComponentVisualizer.h"

#include <Core/BoxColliderComponent.h>
#include <Core/Map.h>
#include <Core/Object.h>
#include <EditorDirector/EditorDebugDraw.h>

void BoxColliderComponentVisualizer::Update(CameraComponent *)
{
    Map *sourceMap = GetSourceMap();
    Map *editorMap = GetEditorMap();
    if (!sourceMap || !editorMap)
        return;

    // 프리팹 추가/제거는 일반 object 알림을 거치지 않는 경로도 있다.
    // 현재 목록을 직접 조회해 런타임 컴포넌트 추가와 지연 삭제를 함께 처리한다.
    for (Object *object : sourceMap->GetEntityList())
    {
        if (!object || object->GetKillState() || !object->GetActive() || object->GetMap() != sourceMap)
            continue;

        for (Component *component : object->GetComponentList())
        {
            if (!component || component->GetDeadState())
                continue;

            auto collider = dynamic_cast<BoxColliderComponent *>(component);
            if (!collider || collider->GetMap() != sourceMap)
                continue;

            // 물리를 꺼 둔 상태에서도 형상을 편집할 수 있어야 하므로 body 등록 여부는 검사하지 않는다.
            EditorDebugDraw::DrawBoxColliderVisual(editorMap, collider);
        }
    }
}

void BoxColliderComponentVisualizer::UnBindMap()
{
    // 별도의 캐시/시각화 객체가 없으므로 source map 참조 해제만으로 다음 제출을 중단한다.
    BindMap(nullptr);
}
