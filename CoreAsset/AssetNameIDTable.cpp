#include "AssetIDTable.h"
#include "TextureManager.h"

CoreAsset::AssetIDTable::AssetIDTable()
{
}

CoreAsset::AssetIDTable::~AssetIDTable()
{
}

bool CoreAsset::AssetIDTable::HasName(const std::string& name) const
{

	AssetNameIDTableType::const_iterator it = mNameIDTable.find(name);

	return it == mNameIDTable.cend() ? false : true;

}

bool CoreAsset::AssetIDTable::HasID(AssetID id) const
{
	AssetIDPtrTableType::const_iterator it = mIDPtrTable.find(id);

	return it == mIDPtrTable.cend() ? false : true;

}

CoreAsset::AssetID CoreAsset::AssetIDTable::GetAssetID(const std::string& name) const
{

	AssetNameIDTableType::const_iterator it = mNameIDTable.find(name);


	return  it == mNameIDTable.cend() ? NoneAssetID : it->second;
}

const std::string CoreAsset::AssetIDTable::GetAssetName(AssetID id) const
{
	
	AssetIDNameTableType::const_iterator it = mIDNameTable.find(id);
	if (it == mIDNameTable.cend())
		return "";

	
	return it->second;

}


CoreAsset::Asset* CoreAsset::AssetIDTable::GetAsset(AssetID id) const
{

	AssetIDPtrTableType::const_iterator it = mIDPtrTable.find(id);
	if (it == mIDPtrTable.cend())
		return nullptr;


	return it->second;
}

CoreAsset::Asset* CoreAsset::AssetIDTable::GetAsset(const std::string& name) const
{


	AssetID id = GetAssetID(name);
	if (id == NoneAssetID)
		return nullptr;

	return GetAsset(id);

}

bool CoreAsset::AssetIDTable::SetAsset(
	const std::string& name, CoreAsset::AssetID id, CoreAsset::Asset* asset)
{
	Asset* checkAsset = GetAsset(name);
	if (checkAsset != nullptr || checkAsset == asset )
		return false;

	checkAsset = GetAsset(id);
	if (checkAsset != nullptr  || checkAsset == asset)
		return false;




	mNameIDTable[name] = id;
	mIDNameTable[id] = name;
	mIDPtrTable[id] = asset;

	return true;
}


