#include "IUIComponent.h"
#include <ReflectSystem/ReflectionSystem.h>

#include <ReflectSystem/ReflectionClassInfo.h>
#include <cstddef>

struct IUIComponentRegistrar {
    IUIComponentRegistrar()    {
Quad::ReflectionSystem* reflectionSystem = Quad::ReflectionSystem::GetInstance(); 
Quad::ClassInfo* cls = reflectionSystem->CreateClassInfo("IUIComponent" ,sizeof(UI::IUIComponent) , nullptr , nullptr); 
cls->SetBaseClassList("",0); 
    }
};
static IUIComponentRegistrar G_IUIComponentRegistrar;
