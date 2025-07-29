#pragma once




#include"CallbackDllMacro.h"
#include"ICallbackSystemOneParamImpl.h"
#include"Allocator.h"

#include"CallbackSystemHandle.h"


#include <type_traits>


template<typename T,typename Input>
struct CallbackThunkOneParam
{
	T* mInstance;
	void (T::*callback)(Input);

	~CallbackThunkOneParam()= default;


	void fun(Input param1)
	{
		(mInstance->*callback)(param1);
	}
};


template<typename Input>
struct GlobalGallbackThunk
{
	void (*callback)(Input );


	~GlobalGallbackThunk() = default;
	
	void fun(Input param1)
	{
		callback(param1);
	}
};



template<typename T, typename Input>
void GenericCallbackThunkOneParam(void * thunk, void * input)
{
	CallbackThunkOneParam<T, Input>* callbackThunk = reinterpret_cast<CallbackThunkOneParam<T, Input>*>(thunk);
	callbackThunk->fun((Input)input);
}


template <typename Input>
void GenericGlobalCallbackThunkOneParam(void* thunk, void* input)
{
	GlobalGallbackThunk<Input>* callbackThunk = reinterpret_cast<GlobalGallbackThunk<Input>*>(thunk);
	callbackThunk->fun((Input)input);
}



template<typename T,typename Input>
void GenericDestroyCallbackThunkOneParam(void* thunk)
{
	CallbackThunkOneParam<T, Input>* callbackThunk = reinterpret_cast<CallbackThunkOneParam<T, Input>*>(thunk);

	callbackThunk->~CallbackThunkOneParam<T, Input>();

	Allocator::Release(callbackThunk);

}


template <typename Input>
void GenericGlobalDestroyCallbackThunkOneParam(void* thunk)
{
	GlobalGallbackThunk<Input>* callbackThunk = reinterpret_cast<GlobalGallbackThunk<Input>*>(thunk);

	callbackThunk->~GlobalGallbackThunk<Input>();

	Allocator::Release(callbackThunk);
}





template<typename Input>
class CallbackSystemOneParam
{

public:
	CallbackSystemOneParam();
	~CallbackSystemOneParam();


	template<typename T>
	CallbackSystemHandle RegisterCallback(void (T::*callback)(Input), T* instance);
	CallbackSystemHandle RegisterCallback(void (*callback)(Input));

	
	void Call(Input input);


	//등록된 콜백 모두 제거
	//싱글 콜백시스템일경우 존재하는 Reset메서드
	void Reset();


	//특정콜백제거
	void ReleaseCallback(CallbackSystemHandle callbackHandle);



private:

	ICallbackSystemOneParamImpl* mImpl;


};

template <typename Input>
inline CallbackSystemOneParam<Input>::CallbackSystemOneParam()
{
	mImpl = CreateCallbackSystemOneParamImpl();
}

template <typename Input>
inline CallbackSystemOneParam<Input>::~CallbackSystemOneParam()
{
}


template <typename Input>
template<typename T>
CallbackSystemHandle CallbackSystemOneParam<Input>::RegisterCallback(void (T::*callback)(Input), T* instance)
{
	//메서드 등록


	void* memPtr = Allocator::Alloc(sizeof(CallbackThunkOneParam<T, Input>));

	CallbackThunkOneParam<T, Input>* chunk = new (memPtr) CallbackThunkOneParam<T, Input>();

	chunk->mInstance = instance;
	chunk->callback = callback;




	Object* object = nullptr;
	//Object 계통의 클래스 여부 확인
	if constexpr (std::is_base_of_v<Object, T>)
	{

		object = instance;

	}

	return mImpl->Register(&GenericCallbackThunkOneParam<T,Input>, chunk, &GenericDestroyCallbackThunkOneParam<T,Input>,object);
}




template<typename Input>
CallbackSystemHandle CallbackSystemOneParam<Input>::RegisterCallback(void (*callback)(Input))
{
	//일반함수 등록
	void* memPtr = Allocator::Alloc(sizeof(GlobalGallbackThunk<Input>));

	GlobalGallbackThunk<Input>* chunk = new (memPtr) GlobalGallbackThunk<Input>();
	chunk->callback = callback;

	return mImpl->Register(&GenericGlobalCallbackThunkOneParam<Input>, chunk, &GenericGlobalDestroyCallbackThunkOneParam<Input>,nullptr);
}

template <typename Input>
inline void CallbackSystemOneParam<Input>::Call(Input input)
{
	mImpl->Call((void*)input);
}

template <typename Input>
inline void CallbackSystemOneParam<Input>::Reset()
{

	mImpl->Reset();
}

template <typename Input>
inline void CallbackSystemOneParam<Input>::ReleaseCallback(CallbackSystemHandle callbackHandle)
{
	mImpl->Release(callbackHandle);
}






