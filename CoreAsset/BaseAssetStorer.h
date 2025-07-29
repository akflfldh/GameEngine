#pragma once

#include<vector>
#include<fstream>
#include<BinaryWriter.h>
#include"CoreAssetDLLMacro.h"
namespace CoreAsset
{
	class Asset;
	class AssetMetaData;

	class CORE_ASSET_API BaseAssetStorer
	{
	public:
		BaseAssetStorer();
		virtual ~BaseAssetStorer() = 0;


	protected:
		QuadRW::BinaryWriter mWriter;


	};


}
