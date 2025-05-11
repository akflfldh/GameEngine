#pragma once

#include"Predeclare.h"

#include<string>
#include"Core/CoreDllExport.h"
namespace Quad

{

	class CORE_API_LIB EffectUtility
	{
	public:
		EffectUtility() = delete;
		~EffectUtility()=default;

		static int ConvertObjectOrPassFlag(const std::wstring& flag);


	private:
	};
}

