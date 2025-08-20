#include "UIElement.h"
#include <ReflectSystem/ReflectionSystem.h>

#include <ReflectSystem/ReflectionClassInfo.h>
#include <cstddef>

struct UIElementRegistrar {
    UIElementRegistrar()    {
Quad::ReflectionSystem* reflectionSystem = Quad::ReflectionSystem::GetInstance(); 
Quad::ClassInfo* cls = reflectionSystem->CreateClassInfo("UIElement" ,sizeof(UI::UIElement) , [](void* mem) { return (void*)(new (mem) UI::UIElement); } , [](void* instance) { ((UI::UIElement*)(instance))->~UIElement(); }); 
cls->SetBaseClassList("",0); 
reflectionSystem->AddProperty(cls, Quad::PropertyInfo{"A", "int" , 292}); 
reflectionSystem->AddProperty(cls, Quad::PropertyInfo{"b", "int" , 296}); 
    }
};
static UIElementRegistrar G_UIElementRegistrar;
