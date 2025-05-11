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



#define CLASSNAME(ObjectClassName) \
virtual const char * GetClassTypeName() const {	return #ObjectClassName;}



#define CLASSSIZE(ObjectClassName)\
virtual const unsigned int GetClassSize() const { return sizeof(ObjectClassName);}