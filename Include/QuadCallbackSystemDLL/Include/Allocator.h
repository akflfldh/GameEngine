#pragma once


#include"CallbackDllMacro.h"


class QUAD_CALLBACK_SYSTEM_API Allocator
{

public:


	//향후정렬필요
	static void* Alloc(size_t size);


	static void Release(void* ptr);



private:

};
