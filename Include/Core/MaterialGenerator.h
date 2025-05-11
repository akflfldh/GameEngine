#pragma once

#include"RawResourceType.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB MaterialGenerator
	{

	public:
		MaterialGenerator() = delete;
		~MaterialGenerator() = delete;

		static bool CreateDefaultMaterial(RawMaterialData& oRawMaterialData);


	private:

	};

}