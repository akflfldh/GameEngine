#include "Core/InputLayoutTable.h"

std::unordered_map<std::wstring, D3D12_INPUT_LAYOUT_DESC* > Quad::InputLayoutTable::mInputLayoutTable;

D3D12_INPUT_LAYOUT_DESC* Quad::InputLayoutTable::GetInputLayout(const std::wstring& name)
{
    std::unordered_map<std::wstring, D3D12_INPUT_LAYOUT_DESC*>::iterator it =
        mInputLayoutTable.find(name);

    return it!=mInputLayoutTable.end()? it->second : nullptr;
}

bool Quad::InputLayoutTable::AddInputLayout(const std::wstring& name, D3D12_INPUT_LAYOUT_DESC* layout)
{
    std::pair<std::unordered_map<std::wstring, D3D12_INPUT_LAYOUT_DESC*>::iterator, bool>  ret =
        mInputLayoutTable.insert({ name,layout });
    return ret.second;
}
