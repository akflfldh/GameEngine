#pragma once



#include"Asset/Asset.h"
#include"Core/CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB IAssetSmartPointer
	{

	public:
		IAssetSmartPointer() = default;
		virtual ~IAssetSmartPointer() =default;

	
		virtual Asset* GetPointer(bool checkTableFlag = false) const = 0;
		
	private:


	};

}