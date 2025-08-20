#include "UIButtonComponent.h"
#include <ReflectSystem/ReflectionSystem.h>

#include <ReflectSystem/ReflectionClassInfo.h>
#include <cstddef>

struct UIButtonComponentRegistrar {
    UIButtonComponentRegistrar()    {
Quad::ReflectionSystem* reflectionSystem = Quad::ReflectionSystem::GetInstance(); 
Quad::ClassInfo* cls = reflectionSystem->CreateClassInfo("UIButtonComponent" ,sizeof(UI::UIButtonComponent) , [](void* mem) { return (void*)(new (mem) UI::UIButtonComponent); } , [](void* instance) { ((UI::UIButtonComponent*)(instance))->~UIButtonComponent(); }); 
cls->SetBaseClassList("IUIComponent ",1); 
    }
};
static UIButtonComponentRegistrar G_UIButtonComponentRegistrar;
