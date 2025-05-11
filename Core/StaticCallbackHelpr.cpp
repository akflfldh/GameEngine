#include "StaticCallbackHelpr.h"
#include"Core/InputSystemImpl.h"

namespace Quad
{

	void StaticCallbackRegisterHelper::registerCallbackStruct(const std::string& behavior, EInputState inputState, const CallbackStruct& callbackStruct)
	{
		InputSystemImpl* inputSystemImpl = InputSystemImpl::GetInstance();
		inputSystemImpl->RegisterActionCallback(behavior, inputState, callbackStruct);
	}

	void StaticCallbackRegisterHelper::ReleaseCallbackStruct(const std::string& behavior, EInputState inputState, const CallbackStruct& callbackStruct)
	{
		InputSystemImpl* inputSystemImpl = InputSystemImpl::GetInstance();
		inputSystemImpl->ReleaseActionCallback(behavior, inputState, callbackStruct);
	}

	void StaticCallbackRegisterHelper::RegisterAxisCallbackStruct(const std::string& behavior, const AxisCallbackStruct& callbackStruct)
	{
		InputSystemImpl* inputSystemImpl = InputSystemImpl::GetInstance();
		inputSystemImpl->RegisterAxisCallback(behavior, callbackStruct);

	}


	void StaticCallbackRegisterHelper::ReleaseAxisCallbackStruct(const std::string& behavior, const AxisCallbackStruct& callbackStruct)
	{
		InputSystemImpl* inputSystemImpl = InputSystemImpl::GetInstance();
		inputSystemImpl->ReleaseAxisCallback(behavior, callbackStruct);
	}

	void StaticCallbackRegisterHelper::registerInputContextCallbackStruct(const std::string& behavior, EInputState inputState, const InputContextCallbackStruct& callbackStruct)
	{


		InputSystemImpl* inputSystemImpl = InputSystemImpl::GetInstance();

		inputSystemImpl->RegisterActionInputContextCallback(behavior, inputState, callbackStruct);
	}

	void StaticCallbackRegisterHelper::ReleaseInputContextCallbackStruct(const std::string& behavior, EInputState inputState, const InputContextCallbackStruct& callbackStruct)
	{
		InputSystemImpl* inputSystemImpl = InputSystemImpl::GetInstance();
		inputSystemImpl->ReleaseActionInputContextCallback(behavior, inputState, callbackStruct);
	}

}