#pragma once

#include"CallbackDllMacro.h"
#include"CallbackSystemHandle.h"


class Object;
class ICallbackSystemOneParamImpl
{

	public:
	ICallbackSystemOneParamImpl();
	virtual ~ICallbackSystemOneParamImpl() = 0;

	//만약 Object계통의 클래스 인스턴스라면 Weak_ptr을통해 인스턴스 소멸시 자동으로 안전하게 등록된 콜백을 해제한다. 
	//따라서 object를 매개변수로 추가로 넘긴다.
	//그외 클래스타입, 일반함수의 경우는 nullptr을 전달한다.
	virtual CallbackSystemHandle Register(void (*genericCallback)(void*, void*), void* chunk, void (*destoryCallback)(void*)) = 0;
	virtual void				 Release(CallbackSystemHandle handle) = 0; 


	virtual void Call(void* input) = 0 ;

	virtual void Reset() = 0;

	private:
	
};


QUAD_CALLBACK_SYSTEM_API ICallbackSystemOneParamImpl* CreateCallbackSystemOneParamImpl();