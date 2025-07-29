#include "AssetCommon.h"



#include"Asset.h"
#include<vector>
#include<Utility.h>

const char* CoreAsset::GetAssetFileExtension()
{
	return "asset";
}

const char* CoreAsset::GetAssetRawFileExtension()
{
	return "raw";
}

std::string CoreAsset::GetAssetFileName(CoreAsset::Asset* asset)
{
	if (asset == nullptr)
		return "";




	return asset->GetName() + "."+ GetAssetFileExtension();
	


}

std::string CoreAsset::GetAssetRawFileName(const std::string & logicalPath)
{

	std::vector<std::string> logicalPathTokenVector=  CoreUtility::Utility::Split(logicalPath, '/');


	std::string rawFileName;
	for (const auto& folderToken : logicalPathTokenVector)
	{
		rawFileName += folderToken + ".";
	}

	rawFileName += GetAssetRawFileExtension();


	return rawFileName;
}





