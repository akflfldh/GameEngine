#pragma once

#pragma once
#include"Core/InputSystemType.h"

#include<string>

namespace Quad
{
	class StaticCallbackRegisterHelper
	{
	public:

		static void registerCallbackStruct(const std::string& behavior, EInputState inputState, const CallbackStruct& callbackStruct);
		static void ReleaseCallbackStruct(const std::string& behavior, EInputState inputState, const CallbackStruct& callbackStruct);


		static void RegisterAxisCallbackStruct(const std::string& behavior, const AxisCallbackStruct& callbackStruct);
		static void ReleaseAxisCallbackStruct(const std::string& behavior, const AxisCallbackStruct& callbackStruct);


		static void registerInputContextCallbackStruct(const std::string& behavior, EInputState inputState, const InputContextCallbackStruct& callbackStruct);
		static void ReleaseInputContextCallbackStruct(const std::string& behavior, EInputState inputState, const InputContextCallbackStruct& callbackStruct);

	private:

	};



}
