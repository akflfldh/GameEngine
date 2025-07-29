#pragma once

#include<stdint.h>
#include<string>

#include<AssetType.h>

namespace QuadLF
{

	using LogicalNodeID = uint32_t;


	enum class ELogicalNodeType
	{
		eFolder=0,
		eFile
	};



	struct LogicalFileAssetInfo
	{
		CoreAsset::AssetID mAssetID;
		std::string mName;
		CoreAsset::EAssetType mAssetType;
	};



}