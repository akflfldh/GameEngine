#pragma once


#include<string>
#include<unordered_map>
#include<memory>
#include"InputType.h"
#include"ICallbackTable.h"
#include"IAxisCallbackTable.h"

namespace Quad
{


	class InputSystemImpl
	{
	public:
		InputSystemImpl();
		~InputSystemImpl();

		void Update(float deltaTime);

		bool CreateNewEnvironment(const std::string& name);
		bool SetCurrentEnvironment(const std::string& name);
		void SetNullEnvironment();


		//action
		void RegisterKeyValue(int key, EInputState keyState, const std::string& behaviorName);

		template<typename T>
		void RegisterBehaviorCallback(const std::string& behaviorName, EInputState keyState, T* object, void(T::* callback)(void));


		template<typename T>
		void ReleaseBehaviorCallback(const std::string& behaviorName, EInputState keyState, T* object, void(T::* callback)(void));


		//axis
		void RegisterAxisKeyValue(const std::string& behaviorName, std::pair<int, float> keyValue);

		template<typename T>
		void RegisterBehaviorAxisCallback(const std::string& behaviorName, T* object, void(T::*)(float));

		template<typename T>
		void ReleaseBehaviorAxisCallback(const std::string& behaviorName, T* object, void (T::*)(float));


		void OnEvent(int key, EInputState inputStat);




		void ReBindActionKey(int beforeKey, EInputState inputState, const std::string& behaviorName, int newKey);



	private:

		void UpdateKeyHeld(float deltaTime);



		//단발 action
								//<key value - <inputState - behaviorName>>
		using KeyBehaviorMap = std::unordered_map<int, std::unordered_map<EInputState, std::string>>;

		//<behaviorName - <inputState - callbackList>>
		using BehaviorCallbackMap = std::unordered_map<std::string, std::unordered_map<EInputState, std::vector<std::unique_ptr<ICallbackTable>>>>;

		//연속 action
		using BehaviorKeyValueMap = std::unordered_map<std::string, std::vector<std::pair<int, float>>>;
		using BehaviorAxisCallbackMap = std::unordered_map<std::string, std::vector<std::unique_ptr<IAxisCallbackTable>>>;


		struct InputTableStruct
		{

			KeyBehaviorMap mKeyBehaviorMappingTable;
			BehaviorCallbackMap mBehaviorCallbackMappingTable;
			std::vector< KeyHeldStruct> mKeyHeldVector;

			BehaviorKeyValueMap mBehaviorKeyValueAxisMappingTable;
			BehaviorAxisCallbackMap mBehaviorAxisCallbackMappingTable;


		};

		std::unordered_map<std::string, std::unique_ptr<InputTableStruct>> mInputEnvironmentTable;

		InputTableStruct* mCurrentInputTableStruct;






		int mKey;




	};


	template<typename T>
	inline void InputSystemImpl::RegisterBehaviorCallback(const std::string& behaviorName, EInputState keyState, T* object, void(T::* callback)(void))
	{

		BehaviorCallbackMap& behaviorCallbackMappingTable = mCurrentInputTableStruct->mBehaviorCallbackMappingTable;



		//dynamic_cast
		std::unordered_map<EInputState, std::vector<std::unique_ptr<ICallbackTable>>>& inputStateCallbackMappingTable = behaviorCallbackMappingTable[behaviorName];

		std::vector<std::unique_ptr<ICallbackTable>>& callbackTableVector = inputStateCallbackMappingTable[keyState];

		for (auto& callbackTable : callbackTableVector)
		{
			if (dynamic_cast<CallbackTable<T>*>(callbackTable.get()) != nullptr)
			{
				CallbackTable<T>* derivedCallbackTable = static_cast<CallbackTable<T>*>(callbackTable.get());
				derivedCallbackTable->Register(object, callback);
				return;
			}
		}

		//없는경우
		auto newCallbackTable = std::make_unique<CallbackTable<T>>();
		newCallbackTable->Register(object, callback);

		callbackTableVector.push_back(std::move(newCallbackTable));

	}

	template<typename T>
	inline void InputSystemImpl::ReleaseBehaviorCallback(const std::string& behaviorName, EInputState keyState, T* object, void(T::* callback)(void))
	{
		BehaviorCallbackMap& behaviorCallbackMappingTable = mCurrentInputTableStruct->mBehaviorCallbackMappingTable;


		std::unordered_map<EInputState, std::vector< std::unique_ptr<ICallbackTable>>>& InputStateCallbackMappingTable = behaviorCallbackMappingTable[behaviorName];
		std::vector< std::unique_ptr<ICallbackTable>>& callbackTableVector = InputStateCallbackMappingTable[keyState];


		for (auto& callbackTable : callbackTableVector)
		{
			if (dynamic_cast<CallbackTable<T>*>(callbackTable.get()) != nullptr)
			{
				CallbackTable<T>* derivedCallbackTable = static_cast<CallbackTable<T>*>(callbackTable.get());

				derivedCallbackTable->Release(object, callback);

				//만약 이 table에 등록된 callback의 개수가 0이면 이 callbacktable을 지운다.
				//또 만약 이 callbackTableVector의 개수가 0이고, InputState가 keyHeld라면 mKeyHeldVector에서 그 key에 해당하는 요소를 지운다.

				return;
			}
		}


	}

	template<typename T>
	inline void InputSystemImpl::RegisterBehaviorAxisCallback(const std::string& behaviorName, T* object, void(T::* callback)(float))
	{

		BehaviorAxisCallbackMap& behaviorCallbackMappingTable = mCurrentInputTableStruct->mBehaviorAxisCallbackMappingTable;

		std::vector<std::unique_ptr<IAxisCallbackTable>>& callbackTableVector = behaviorCallbackMappingTable[behaviorName];

		for (auto& element : callbackTableVector)
		{

			if (dynamic_cast<AxisCallbackTable<T>*>(element.get()) != nullptr)
			{
				AxisCallbackTable<T>* derivedCallbackTable = static_cast<AxisCallbackTable<T>*>(element.get());

				derivedCallbackTable->Register(object, callback);
				return;
			}

		}


		auto newCallbackTable = std::make_unique<AxisCallbackTable<T>>();

		newCallbackTable->Register(object, callback);


		callbackTableVector.push_back(std::move(newCallbackTable));

	}

	template<typename T>
	inline void InputSystemImpl::ReleaseBehaviorAxisCallback(const std::string& behaviorName, T* object, void(T::* callback)(float))
	{
		BehaviorAxisCallbackMap& behaviorCallbackMappingTable = mCurrentInputTableStruct->mBehaviorAxisCallbackMappingTable;

		typename  BehaviorAxisCallbackMap::iterator  it = behaviorCallbackMappingTable.find(behaviorName);

		if (it == behaviorCallbackMappingTable.end())
			return;


		std::vector<std::unique_ptr<IAxisCallbackTable>>& callbackTableList = it->second;

		for (auto& callbackTableElement : callbackTableList)
		{

			if (dynamic_cast<AxisCallbackTable<T>*>(callbackTableElement.get()) != nullptr)
			{
				AxisCallbackTable<T>* callbackTable = static_cast<AxisCallbackTable<T>*>(callbackTableElement.get());

				callbackTable->Release(object, callback);
				return;

			}


		}



	}



}