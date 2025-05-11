#pragma once


#include"RegisterEditorObjectHelper.h"
#include"ObjectBinnedAllocator.h"

#define REGISTEREDITORCLASS(ObjectClassName)																						\
inline Quad::RegisterEditorObjectHelper registerEditorObjectHelper##ObjectClassName(#ObjectClassName, []()->Object* {				\
	void* addr = GetObjectBinnedAllocator(sizeof(ObjectClassName))->Alloc(); ;														\
	Object* newObject = new(addr) ObjectClassName;																					\
	return newObject;																												\
																																	\
	},																																\
	[](Object* ob) {		 GetObjectBinnedAllocator(sizeof(ObjectClassName))->Release(ob); }										\
																																	\
); 
