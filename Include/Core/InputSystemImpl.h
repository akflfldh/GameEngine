#pragma once



#include<unordered_map>
#include<string>
#include<memory>
#include"InputSystemType.h"


namespace Quad
{


	class InputSystemImpl
	{

	public:
		~InputSystemImpl();

		static InputSystemImpl* GetInstance();


		void Update(float deltaTime);


		void RegisterActionKeyBehavior(int key, EInputState inputState, const std::string& behaviorName);
		void RegisterActionCallback(const std::string& behaviorName, EInputState inputState, const CallbackStruct& callback);
		void ReleaseActionCallback(const std::string& behaviorName, EInputState inputState, const CallbackStruct& callback);

		void RegisterActionInputContextCallback(const std::string& behaviorName, EInputState inputState, const InputContextCallbackStruct& callback);
		void ReleaseActionInputContextCallback(const std::string& behaviorName, EInputState inputState, const InputContextCallbackStruct& callback);


		void OnEvent(int key, EInputState inputState);




		void RegisterAxisKeyBehavior(int key, float value, const std::string& behaviorName);
		void ChangeAxisKeyValue(int key, float value, const std::string behaviorName);
		void RegisterAxisCallback(const std::string& behaviorName, const AxisCallbackStruct& callback);
		void ReleaseAxisCallback(const std::string& behaviorName, const AxisCallbackStruct& callback);


		void RegisterEnvironment(const std::string& name);
		void ChangeEnvironment(const std::string& name);


	private:
		InputSystemImpl();

	

		void UpdateAxisCallback(float deltaTime);
		void UpdateKeyHeldBehavior(float deltaTime);
		float IsKeyDown(int key);

		const std::vector<std::pair<int, float>>::iterator GetKeyValueIt(const std::string& behaviorName, int key);







		using KeyInputStateBehaviorMap = std::unordered_map<int, std::unordered_map<EInputState, std::string>>;
		using InputStateBehaviorMap = std::unordered_map<EInputState, std::string >;

		using BehaviorInputStateCallbackMap = std::unordered_map<std::string, std::unordered_map<EInputState, std::vector<CallbackStruct>>>;
		using InputStateCallbackMap = std::unordered_map<EInputState, std::vector<CallbackStruct>>;


		using BehaviorInputStateContextCallbackMap = std::unordered_map<std::string, std::unordered_map<EInputState, std::vector<InputContextCallbackStruct>>>;
		using InputStateInputContextCallbackMap = std::unordered_map<EInputState, std::vector<InputContextCallbackStruct>>;



		//axis
												//<behavior, <key, value>>
		using BehaviorKeyMap = std::unordered_map<std::string, std::vector<std::pair<int, float>>>;
		using BehaviorCallbackMap = std::unordered_map<std::string, std::vector<AxisCallbackStruct>>;



		struct InputTableStruct
		{
			//단발 action
			KeyInputStateBehaviorMap mKeyBehaviorActionTable;
			BehaviorInputStateCallbackMap mBehaviorCallbackActionTable;
			BehaviorInputStateContextCallbackMap mBehaviorInputContextCallbackActionTable;


			std::vector<KeyHeldStruct> mKeyHeldStructVector;

			//연속 axis
			BehaviorKeyMap mBehaviorKeyAxisTable;
			BehaviorCallbackMap mBehaviorCallbackAxisTable;
		};


	private:

		void OnEventActionCallback(int key, EInputState inputState, const std::string& behaviorName, BehaviorInputStateCallbackMap& behaviorCallbackActionTable);
		void OnEventActionContextCallback(int key, EInputState inputState, const std::string& behaviorName, BehaviorInputStateContextCallbackMap& behaviorInputContextCallbackActionTable);




		//<environment ,  inputTableStruct >
		std::unordered_map<std::string, std::unique_ptr<InputTableStruct>> mInputEnvironmentTable;

		InputTableStruct* mCurrentInputTable;




	};



}

