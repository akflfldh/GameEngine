#include "UIResizeGizmoRenderableComponent.h"
#include <ReflectSystem/ReflectionSystem.h>

#include <ReflectSystem/ReflectionClassInfo.h>
#include <cstddef>

struct UIResizeGizmoRenderableComponentRegistrar {
    UIResizeGizmoRenderableComponentRegistrar()    {
Quad::ReflectionSystem* reflectionSystem = Quad::ReflectionSystem::GetInstance(); 
Quad::ClassInfo* cls = reflectionSystem->CreateClassInfo("UIResizeGizmoRenderableComponent" ,sizeof(UI::UIResizeGizmoRenderableComponent) , [](void* mem) { return (void*)(new (mem) UI::UIResizeGizmoRenderableComponent); } , [](void* instance) { ((UI::UIResizeGizmoRenderableComponent*)(instance))->~UIResizeGizmoRenderableComponent(); }); 
cls->SetBaseClassList("UIRenderableComponent IUIComponent ",2); 
    }
};
static UIResizeGizmoRenderableComponentRegistrar G_UIResizeGizmoRenderableComponentRegistrar;
