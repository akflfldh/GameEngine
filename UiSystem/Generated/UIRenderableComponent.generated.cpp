#include "UIRenderableComponent.h"
#include <ReflectSystem/ReflectionSystem.h>

#include <ReflectSystem/ReflectionClassInfo.h>
#include <cstddef>

struct UIRenderableComponentRegistrar {
    UIRenderableComponentRegistrar()    {
Quad::ReflectionSystem* reflectionSystem = Quad::ReflectionSystem::GetInstance(); 
Quad::ClassInfo* cls = reflectionSystem->CreateClassInfo("UIRenderableComponent" ,sizeof(UI::UIRenderableComponent) , nullptr , nullptr); 
cls->SetBaseClassList("IUIComponent ",1); 
    }
};
static UIRenderableComponentRegistrar G_UIRenderableComponentRegistrar;
