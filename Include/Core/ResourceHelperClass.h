#pragma once

#include"Predeclare.h"
#include<string>
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB ResourceHelperClass
	{
	public:
		~ResourceHelperClass();


		static const std::string GetTextureFileName(Texture* texture);




	private:
		ResourceHelperClass() = delete;
	};

}