#include "UIMovableComponent.h"
#include <ReflectSystem/ReflectionSystem.h>

#include <ReflectSystem/ReflectionClassInfo.h>
#include <cstddef>

struct UIMovableComponentRegistrar {
    UIMovableComponentRegistrar()    {
Quad::ReflectionSystem* reflectionSystem = Quad::ReflectionSystem::GetInstance(); 
Quad::ClassInfo* cls = reflectionSystem->CreateClassInfo("UIMovableComponent" ,sizeof(UI::UIMovableComponent) , [](void* mem) { return (void*)(new (mem) UI::UIMovableComponent); } , [](void* instance) { ((UI::UIMovableComponent*)(instance))->~UIMovableComponent(); }); 
cls->SetBaseClassList("IUIComponent ",1); 
    }
};
static UIMovableComponentRegistrar G_UIMovableComponentRegistrar;
