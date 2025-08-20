#include "UIButton.h"
#include <ReflectSystem/ReflectionSystem.h>

#include <ReflectSystem/ReflectionClassInfo.h>
#include <cstddef>

struct UIButtonRegistrar {
    UIButtonRegistrar()    {
Quad::ReflectionSystem* reflectionSystem = Quad::ReflectionSystem::GetInstance(); 
Quad::ClassInfo* cls = reflectionSystem->CreateClassInfo("UIButton" ,sizeof(UI::UIButton) , [](void* mem) { return (void*)(new (mem) UI::UIButton); } , [](void* instance) { ((UI::UIButton*)(instance))->~UIButton(); }); 
cls->SetBaseClassList("UIElement ",1); 
    }
};
static UIButtonRegistrar G_UIButtonRegistrar;
