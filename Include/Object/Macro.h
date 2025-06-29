#pragma once
#include"Object/RegisterObjectHelper.h"
#include"Core/ObjectBinnedAllocator.h"




#define REGISTERCLASS(ObjectClassName)														\
inline Quad::RegisterObjectHelper registerObjectHelper##ObjectClassName(#ObjectClassName,	\
[]()->Quad::Object*{																				\
	void * addr = Quad::GetObjectBinnedAllocator(sizeof(ObjectClassName))->Alloc();	;			\
Quad::Object* newObject = new(addr) ObjectClassName;												\
	return newObject;																		\
																							\
},																							\
[](Quad::Object* ob){	(static_cast<ObjectClassName*>(ob))->~ObjectClassName();	 Quad::GetObjectBinnedAllocator(sizeof(ObjectClassName))->Release(ob);}		\
																							\
);			


#define REGISTERCOMPONENTCLASS(ClassName)																			\
inline Quad::RegisterComponentHelper registerComponentHelper##ClassName(#ClassName,									\
[]()->Quad::BaseComponent * {																						\
			void * addr = Quad::GetObjectBinnedAllocator(sizeof(ClassName))->Alloc();								\
			Quad::BaseComponent* newComponent = new(addr) ClassName;												\
			return newComponent;																					\
},																													\
[](Quad::BaseComponent * component) {																				\
																													\
	(static_cast<ClassName*>(component))->~ClassName();																\
	Quad::GetObjectBinnedAllocator(sizeof(ClassName))->Release(component);}											\
);																													












#define CLASSNAME(ObjectClassName) \
virtual const char * GetClassTypeName() const {	return #ObjectClassName;}


#define CLASSNAMESTATIC(ObjectClassName)\
static const char * GetClassTypeNameStatic() { return #ObjectClassName;}


#define CLASSSIZE(ObjectClassName)\
virtual const unsigned int GetClassSize() const { return sizeof(ObjectClassName);}