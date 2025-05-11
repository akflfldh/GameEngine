#include "Core/InputSystemImpl.h"
#include"Core/KeyBoard.h"
#include"Core/Mouse.h"

namespace Quad
{
	InputSystemImpl::InputSystemImpl()
		:mCurrentInputTable(nullptr)
	{

		mInputEnvironmentTable["Default"] = std::make_unique<InputTableStruct>();
		mCurrentInputTable = mInputEnvironmentTable["Default"].get();
	}

	void InputSystemImpl::OnEventActionCallback(int key, EInputState inputState ,const std::string & behaviorName,BehaviorInputStateCallbackMap & behaviorCallbackActionTable)
	{
		
		BehaviorInputStateCallbackMap::iterator behaviorCallbackTableIt = behaviorCallbackActionTable.find(behaviorName);


		if (behaviorCallbackTableIt == behaviorCallbackActionTable.end())
			return;


		InputStateCallbackMap::iterator inputStateCallbackIt = behaviorCallbackTableIt->second.find(inputState);

		if (inputStateCallbackIt == behaviorCallbackTableIt->second.end())
			return;




		for (auto& callbackStructElement : inputStateCallbackIt->second)
		{
			void* object = callbackStructElement.object;
			callbackStructElement.callback(object);
		}



	}

	void InputSystemImpl::OnEventActionContextCallback(int key, EInputState inputState, const std::string& behaviorName,
		BehaviorInputStateContextCallbackMap& behaviorInputContextCallbackActionTable)
	{
		BehaviorInputStateContextCallbackMap::iterator behaviorInputContextCallbackTableIt = behaviorInputContextCallbackActionTable.find(behaviorName);


		if (behaviorInputContextCallbackTableIt == behaviorInputContextCallbackActionTable.end())
			return;


		InputStateInputContextCallbackMap::iterator inputStateInputContextCallbackIt = behaviorInputContextCallbackTableIt->second.find(inputState);

		if (inputStateInputContextCallbackIt == behaviorInputContextCallbackTableIt->second.end())
			return;



		InputContext inputContext;
		inputContext.key = key;
		inputContext.mState = inputState;
		for (auto& callbackStructElement : inputStateInputContextCallbackIt->second)
		{
			void* object = callbackStructElement.object;
			callbackStructElement.callback(object, inputContext);
		}




	}

	void InputSystemImpl::UpdateAxisCallback(float deltaTime)
	{

		auto& behaviorCallbackAxisTable = mCurrentInputTable->mBehaviorCallbackAxisTable;
		auto& behaviorKeyAxisTable = mCurrentInputTable->mBehaviorKeyAxisTable;



		for (auto& behaviorCallbackElement : behaviorCallbackAxisTable)
		{
			const std::string& behaviorName = behaviorCallbackElement.first;



			std::vector<std::pair<int, float>>& keyValueVector = behaviorKeyAxisTable[behaviorName];
			float totalValue = 0.0f;
			for (auto& keyValueElement : keyValueVector)
			{
				totalValue += keyValueElement.second * IsKeyDown(keyValueElement.first);
			}

			if (totalValue == 0.0f)
				continue;

			std::vector<AxisCallbackStruct>& callbackVector = behaviorCallbackElement.second;
			for (auto& callback : callbackVector)
			{
				callback.callback(callback.object, totalValue);
			}

		}



	}

	void InputSystemImpl::UpdateKeyHeldBehavior(float deltaTime)
	{

		auto& keyHeldStrucVector = mCurrentInputTable->mKeyHeldStructVector;
		auto& keyBehaviorActionTable = mCurrentInputTable->mKeyBehaviorActionTable;
		auto& behaviorCallbackActionTable = mCurrentInputTable->mBehaviorCallbackActionTable;;


		for (auto& keyHeldStructElement : keyHeldStrucVector)
		{
			int key = keyHeldStructElement.key;
			//if (!KeyBoard::IsKeyDown(key))
				//continue;



			keyHeldStructElement.mCurrTime += deltaTime;
			if (keyHeldStructElement.mCurrTime < keyHeldStructElement.mIntervalTime)
				continue;




			KeyInputStateBehaviorMap::iterator keyInputStateBehaviorIt = keyBehaviorActionTable.find(key);
			if (keyInputStateBehaviorIt == keyBehaviorActionTable.end())
				continue;

			InputStateBehaviorMap::iterator inputStateBehaviorIt = keyInputStateBehaviorIt->second.find(EInputState::eKeyHeld);
			if (inputStateBehaviorIt == keyInputStateBehaviorIt->second.end())
				continue;

			const std::string& behaviorName = inputStateBehaviorIt->second;


			BehaviorInputStateCallbackMap::iterator behaviorInputStateCallbackIt = behaviorCallbackActionTable.find(behaviorName);
			if (behaviorInputStateCallbackIt == behaviorCallbackActionTable.end())
				continue;

			InputStateCallbackMap::iterator inputStateCallbackIt = behaviorInputStateCallbackIt->second.find(EInputState::eKeyHeld);
			if (inputStateCallbackIt == behaviorInputStateCallbackIt->second.end())
				continue;

			std::vector<CallbackStruct>& callbackVector = inputStateCallbackIt->second;

			for (auto& callbackElement : callbackVector)
			{
				callbackElement.callback(callbackElement.object);
			}




			keyHeldStructElement.mCurrTime -= keyHeldStructElement.mIntervalTime;



		}






	}

	//눌렀으면 키보드 키들은 1.0f 그렇지 않으면 0,  mouse는 delta이동량
	float InputSystemImpl::IsKeyDown(int key)
	{
		
			switch(key)
			{ 
			case VK_LBUTTON:
				return Mouse::GetLButtonState() ==true ? 1.0f : 0.0f;

			case VK_RBUTTON:
				return Mouse::GetRButtonState() == true? 1.0f :0.0f;
			case kMouseMoveX:
				//mouse moveX
				return Mouse::GetScreenPosDeltaX();
			case kMouseMoveY:
				//mouse moveY
				return Mouse::GetScreenPosDeltaY();
			default:
			{
				return KeyBoard::IsKeyDown(key) == true ? 1.0f : 0.0f;
			}

			}

	
		


	}

	const std::vector<std::pair<int, float>>::iterator InputSystemImpl::GetKeyValueIt(const std::string& behaviorName, int key)
	{

		auto& behaviorKeyAxisTable = mCurrentInputTable->mBehaviorKeyAxisTable;

		std::vector<std::pair<int, float>>& keyValueVector = behaviorKeyAxisTable[behaviorName];

		std::vector<std::pair<int, float>>::iterator it = std::find_if(keyValueVector.begin(), keyValueVector.end(), [key](const std::pair<int, float>& keyValue) {
			if (keyValue.first == key)
				return true;
			return false;

			});

		return it;

	}

	InputSystemImpl::~InputSystemImpl()
	{
	
	}

	InputSystemImpl* InputSystemImpl::GetInstance()
	{
		static InputSystemImpl instance;

		return &instance;
	}

	void InputSystemImpl::Update(float deltaTime)
	{

		UpdateAxisCallback(deltaTime);


		UpdateKeyHeldBehavior(deltaTime);














	}

	void InputSystemImpl::RegisterActionKeyBehavior(int key, EInputState inputState, const std::string& behaviorName)
	{

		auto& keyBehaviorActionTable = mCurrentInputTable->mKeyBehaviorActionTable;
		auto& keyHeldSturctVector = mCurrentInputTable->mKeyHeldStructVector;

		keyBehaviorActionTable[key][inputState] = behaviorName;


		if (inputState == EInputState::eKeyHeld)
		{
			std::vector<KeyHeldStruct>::iterator it = std::find_if(keyHeldSturctVector.begin(), keyHeldSturctVector.end(), [key](const KeyHeldStruct& keyHeldStruct) {
				if (key == keyHeldStruct.key)
					return true;

				return  false;
				});



			if (it == keyHeldSturctVector.end())
			{
				KeyHeldStruct keyHeldStruct;
				keyHeldStruct.key = key;
				keyHeldStruct.mCurrTime = 0.0f;
				keyHeldStruct.mIntervalTime = 1.0f;
				keyHeldSturctVector.push_back(keyHeldStruct);
			}

		}

	}

	void InputSystemImpl::RegisterActionCallback(const std::string& behaviorName, EInputState inputState, const CallbackStruct& callback)
	{






		mCurrentInputTable->mBehaviorCallbackActionTable[behaviorName][inputState].push_back(callback);

	}

	void InputSystemImpl::ReleaseActionCallback(const std::string& behaviorName, EInputState inputState, const CallbackStruct& callback)
	{

		auto& behaviorCallbackActionTable = mCurrentInputTable->mBehaviorCallbackActionTable;


		BehaviorInputStateCallbackMap::iterator beahaviorCallbackIt = behaviorCallbackActionTable.find(behaviorName);
		if (beahaviorCallbackIt == behaviorCallbackActionTable.end())
			return;

		InputStateCallbackMap::iterator inputStateCallbackIt = beahaviorCallbackIt->second.find(inputState);

		if (inputStateCallbackIt == beahaviorCallbackIt->second.end())
			return;



		std::vector<CallbackStruct>& callbackVector = inputStateCallbackIt->second;
		std::vector<CallbackStruct>::iterator it = std::find(callbackVector.begin(), callbackVector.end(), callback);

		if (it != callbackVector.end())
		{
			callbackVector.erase(it);
		}


	}

	void InputSystemImpl::RegisterActionInputContextCallback(const std::string& behaviorName, EInputState inputState, const InputContextCallbackStruct& callback)
	{
		mCurrentInputTable->mBehaviorInputContextCallbackActionTable[behaviorName][inputState].push_back(callback);



	}

	void InputSystemImpl::ReleaseActionInputContextCallback(const std::string& behaviorName, EInputState inputState, const InputContextCallbackStruct& callback)
	{
		BehaviorInputStateContextCallbackMap & behaviorCallbackActionTable = mCurrentInputTable->mBehaviorInputContextCallbackActionTable;

		BehaviorInputStateContextCallbackMap::iterator beahaviorCallbackIt = behaviorCallbackActionTable.find(behaviorName);
		if (beahaviorCallbackIt == behaviorCallbackActionTable.end())
			return;

		InputStateInputContextCallbackMap::iterator inputStateCallbackIt = beahaviorCallbackIt->second.find(inputState);

		if (inputStateCallbackIt == beahaviorCallbackIt->second.end())
			return;



		std::vector<InputContextCallbackStruct>& callbackVector = inputStateCallbackIt->second;
		std::vector<InputContextCallbackStruct>::iterator it = std::find(callbackVector.begin(), callbackVector.end(), callback);

		if (it != callbackVector.end())
		{
			callbackVector.erase(it);
		}


	}

	void InputSystemImpl::OnEvent(int key, EInputState inputState)
	{


		if (inputState == EInputState::eKeyDown)
		{
			auto& keyHeldStructVector = mCurrentInputTable->mKeyHeldStructVector;

			std::vector<KeyHeldStruct>::iterator it = std::find_if(keyHeldStructVector.begin(), keyHeldStructVector.end(), [key](const KeyHeldStruct& keyheldStrcut) {
				if (keyheldStrcut.key == key)
					return true;

				return false;

				});

			if (it != keyHeldStructVector.end())
			{
				it->mCurrTime = 0.0f;
			}
		}


		auto& keyBehaviorActionTable = mCurrentInputTable->mKeyBehaviorActionTable;
		auto& behaviorCallbackActionTable = mCurrentInputTable->mBehaviorCallbackActionTable;
		auto& behaviorInputContextCallbackActionTable = mCurrentInputTable->mBehaviorInputContextCallbackActionTable;


		KeyInputStateBehaviorMap::iterator keyBehaviorIt = keyBehaviorActionTable.find(key);

		if (keyBehaviorIt == keyBehaviorActionTable.end())
			return;


		InputStateBehaviorMap::iterator inputStateBehaviorIt = keyBehaviorIt->second.find(inputState);
		if (inputStateBehaviorIt == keyBehaviorIt->second.end())
			return;



		const std::string& behaivorName = inputStateBehaviorIt->second;






		//callback , inputContextCallback 두콜백들에대해 수행


		//callback 
		OnEventActionCallback(key, inputState, behaivorName, behaviorCallbackActionTable);
		/*BehaviorInputStateCallbackMap::iterator behaviorCallbackTableIt = behaviorCallbackActionTable.find(behaivorName);


		if (behaviorCallbackTableIt == behaviorCallbackActionTable.end())
			return;


		InputStateCallbackMap::iterator inputStateCallbackIt = behaviorCallbackTableIt->second.find(inputState);

		if (inputStateCallbackIt == behaviorCallbackTableIt->second.end())
			return;




		for (auto& callbackStructElement : inputStateCallbackIt->second)
		{
			void* object = callbackStructElement.object;
			callbackStructElement.callback(object);
		}
*/




		//inputContextCallback
		OnEventActionContextCallback(key, inputState, behaivorName, behaviorInputContextCallbackActionTable);
		/*BehaviorInputStateContextCallbackMap::iterator behaviorInputContextCallbackTableIt = behaviorInputContextCallbackActionTable.find(behaivorName);


		if (behaviorInputContextCallbackTableIt == behaviorInputContextCallbackActionTable.end())
			return;


		InputStateInputContextCallbackMap::iterator inputStateInputContextCallbackIt = behaviorInputContextCallbackTableIt->second.find(inputState);

		if (inputStateInputContextCallbackIt == behaviorInputContextCallbackTableIt->second.end())
			return;



		InputContext inputContext;
		inputContext.key = key;
		inputContext.mState = inputState;
		for (auto& callbackStructElement : inputStateInputContextCallbackIt->second)
		{
			void* object = callbackStructElement.object;
			callbackStructElement.callback(object, inputContext);
		}*/























		










	}

	void InputSystemImpl::RegisterAxisKeyBehavior(int key, float value, const std::string& behaviorName)
	{

		auto& behaviorKeyAxisTable = mCurrentInputTable->mBehaviorKeyAxisTable;

		auto it = GetKeyValueIt(behaviorName, key);
		if (it != behaviorKeyAxisTable[behaviorName].end())
			return;

		behaviorKeyAxisTable[behaviorName].push_back({ key,value });

	}

	void InputSystemImpl::ChangeAxisKeyValue(int key, float value, const std::string behaviorName)
	{

		//std::vector<std::pair<int,float>> & keyValueVector =mBehaviorKeyAxisTable[behaviorName];
		//std::vector<std::pair<int, float>>::iterator it = std::find_if(keyValueVector.begin(), keyValueVector.end(), [key](const std::pair<int, float>& keyValue) {
		//	if (keyValue.first == key)
		//		return true;
		//	return false;

		//	});

		//if (it == keyValueVector.end())
		//	return;




		auto it = GetKeyValueIt(behaviorName, key);
		if (it == mCurrentInputTable->mBehaviorKeyAxisTable[behaviorName].end())
			return;


		it->second = value;


	}

	void InputSystemImpl::RegisterAxisCallback(const std::string& behaviorName, const AxisCallbackStruct& callback)
	{

		auto& behaviorCallbackAxisTable = mCurrentInputTable->mBehaviorCallbackAxisTable;

		if (std::find(behaviorCallbackAxisTable[behaviorName].begin(), behaviorCallbackAxisTable[behaviorName].end(), callback) != behaviorCallbackAxisTable[behaviorName].end())
		{
			return;
		}


		behaviorCallbackAxisTable[behaviorName].push_back(callback);
	}

	void InputSystemImpl::ReleaseAxisCallback(const std::string& behaviorName, const AxisCallbackStruct& callback)
	{
		auto& behaviorCallbackAxisTable = mCurrentInputTable->mBehaviorCallbackAxisTable;
		std::vector<AxisCallbackStruct>::iterator it = std::find(behaviorCallbackAxisTable[behaviorName].begin(), behaviorCallbackAxisTable[behaviorName].end(), callback);

		if (it == behaviorCallbackAxisTable[behaviorName].end())
			return;

		behaviorCallbackAxisTable[behaviorName].erase(it);


	}

	void InputSystemImpl::RegisterEnvironment(const std::string& name)
	{

		std::unordered_map<std::string, std::unique_ptr<InputTableStruct>> ::iterator it = mInputEnvironmentTable.find(name);
		if (it != mInputEnvironmentTable.end())
			return;


		mInputEnvironmentTable[name] = std::make_unique< InputTableStruct>();

	}

	void InputSystemImpl::ChangeEnvironment(const std::string& name)
	{

		std::unordered_map<std::string, std::unique_ptr<InputTableStruct>> ::iterator it = mInputEnvironmentTable.find(name);
		if (it == mInputEnvironmentTable.end())
			return;

		mCurrentInputTable = it->second.get();


	}



}