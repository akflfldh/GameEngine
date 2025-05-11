#pragma once

#include<unordered_map>
#include<string>
#include"ICallbackTable.h"
#include"IAxisCallbackTable.h"
#include<memory>


#include"Utility/Singleton.h"
#include"Core/CoreDllExport.h"
#include"Core/InputSystemType.h"
#include"Core/StaticCallbackHelpr.h"



namespace Quad
{


	class InputSystemImpl;

	class CORE_API_LIB InputSystem:public Singleton<InputSystem>
	{
	public:
		InputSystem();
		~InputSystem();

		void Update(float deltaTime);

		void RegisterKeyBehavior(int key, EInputState inputState, const std::string& name);
		void RegisterAxisBehaviorKeyValue(int key, float value, const std::string& behaviorName);

		template<typename T, void(T::* memFun)()>
		void RegisterBehaviorCallbackAction(const std::string& behaviorName, EInputState inputState, T* object);

		template<typename T, void(T::* memFun)()>
		void ReleaseBehaviorCallbackAction(const std::string& behaviorName, EInputState inputState, T* object);




		template<typename T, void(T::* memFun)(float)>
		void RegisterBehaviorCallbackAxis(const std::string& behaviorName, T* object);

		template<typename T, void(T::* memFun)(float)>
		void ReleaseBehaviorCallbackAxis(const std::string& behaviorName, T* object);


		template<typename T, void(T::* memFun)(const InputContext &)>
		void RegisterBehaviorCallbackActionInputContext(const std::string& behaviorName, EInputState inputState, T* object);

		template<typename T, void(T::* memFun)(const InputContext&)>
		void ReleaseBehaviorCallbackActionInputContext(const std::string& behaviorName, EInputState inputState, T* object);







		void OnEvent(int key, EInputState inputState);



		void RegisterEnvironment(const std::string& name);
		void ChangeEnvironment(const std::string& name);

	private:


		InputSystemImpl* mImpl;






	};

	void fun(void*);

	template<typename T, void(T::* memFun)()>
	inline void InputSystem::RegisterBehaviorCallbackAction(const std::string& behaviorName, EInputState inputState, T* object)
	{

		CallbackStruct callbackStruct;

		callbackStruct.object = object;
		callbackStruct.callback = &staticCallback<T, memFun>;

		StaticCallbackRegisterHelper::registerCallbackStruct(behaviorName, inputState, callbackStruct);

	}

	template<typename T, void(T::* memFun)()>
	inline void InputSystem::ReleaseBehaviorCallbackAction(const std::string& behaviorName, EInputState inputState, T* object)
	{
		CallbackStruct callbackStruct;

		callbackStruct.object = object;
		callbackStruct.callback = &staticCallback<T, memFun>;

		StaticCallbackRegisterHelper::ReleaseCallbackStruct(behaviorName, inputState, callbackStruct);

	}

	template<typename T, void(T::* memFun)(float)>
	inline void InputSystem::RegisterBehaviorCallbackAxis(const std::string& behaviorName, T* object)
	{
		AxisCallbackStruct callbackStruct;
		callbackStruct.object = object;
		callbackStruct.callback = &staticAxisCallback<T, memFun>;


		StaticCallbackRegisterHelper::RegisterAxisCallbackStruct(behaviorName, callbackStruct);

	}

	template<typename T, void(T::* memFun)(float)>
	inline void InputSystem::ReleaseBehaviorCallbackAxis(const std::string& behaviorName, T* object)
	{

		AxisCallbackStruct callbackStruct;
		callbackStruct.object = object;
		callbackStruct.callback = &staticAxisCallback<T, memFun>;

		StaticCallbackRegisterHelper::ReleaseAxisCallbackStruct(behaviorName, callbackStruct);
	}

	template<typename T, void(T::* memFun)(const InputContext&)>
	inline void InputSystem::RegisterBehaviorCallbackActionInputContext(const std::string& behaviorName, EInputState inputState, T* object)
	{
		InputContextCallbackStruct callbackStruct;
		callbackStruct.object = object;
		callbackStruct.callback = &staticInputContextCallback<T, memFun>;


		StaticCallbackRegisterHelper::registerInputContextCallbackStruct(behaviorName, inputState, callbackStruct);

	}

	template<typename T, void(T::* memFun)(const InputContext&)>
	inline void InputSystem::ReleaseBehaviorCallbackActionInputContext(const std::string& behaviorName, EInputState inputState, T* object)
	{

		InputContextCallbackStruct callbackStruct;
		callbackStruct.object = object;
		callbackStruct.callback = &staticInputContextCallback<T, memFun>;

		StaticCallbackRegisterHelper::ReleaseInputContextCallbackStruct(behaviorName, inputState,callbackStruct);
	}


}