#include "UIImageComponent.h"
#include <ReflectSystem/ReflectionSystem.h>

#include <ReflectSystem/ReflectionClassInfo.h>
#include <cstddef>

struct UIImageComponentRegistrar {
    UIImageComponentRegistrar()    {
Quad::ReflectionSystem* reflectionSystem = Quad::ReflectionSystem::GetInstance(); 
Quad::ClassInfo* cls = reflectionSystem->CreateClassInfo("UIImageComponent" ,sizeof(UI::UIImageComponent) , [](void* mem) { return (void*)(new (mem) UI::UIImageComponent); } , [](void* instance) { ((UI::UIImageComponent*)(instance))->~UIImageComponent(); }); 
cls->SetBaseClassList("UIRenderableComponent IUIComponent ",2); 
    }
};
static UIImageComponentRegistrar G_UIImageComponentRegistrar;
