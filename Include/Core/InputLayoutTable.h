#pragma once


#include"Predeclare.h"
#include"header.h"
#include"Core/CoreDllExport.h"
namespace Quad
{


	class CORE_API_LIB InputLayoutTable
	{
	public:
		InputLayoutTable() = delete;
		~InputLayoutTable() = default;

		static D3D12_INPUT_LAYOUT_DESC* GetInputLayout(const std::wstring& name);
		static bool AddInputLayout(const std::wstring& name, D3D12_INPUT_LAYOUT_DESC* layout);

	private:
		static std::unordered_map<std::wstring, D3D12_INPUT_LAYOUT_DESC* > mInputLayoutTable;


	};


}

