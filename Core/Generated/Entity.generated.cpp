#include "Entity.h"
#include <ReflectSystem/ReflectionSystem.h>

#include <ReflectSystem/ReflectionClassInfo.h>
#include <cstddef>

struct EntityRegistrar {
    EntityRegistrar()    {
Quad::ReflectionSystem* reflectionSystem = Quad::ReflectionSystem::GetInstance(); 
Quad::ClassInfo* cls = reflectionSystem->CreateClassInfo("Entity" ,sizeof(Core::Entity) , [](void* mem) { return (void*)(new (mem) Core::Entity); } , [](void* instance) { ((Core::Entity*)(instance))->~Entity(); }); 
cls->SetBaseClassList("Object ",1); 
    }
};
static EntityRegistrar G_EntityRegistrar;
