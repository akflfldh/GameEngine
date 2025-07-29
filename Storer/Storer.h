#pragma once

#include<string>

namespace CoreAsset {
	class Asset;
	class AssetMetaData;

}

namespace QuadSR
{
	class TextureStorer;

	class Storer
	{
	public:
		static Storer* GetInstance();
		~Storer();


		bool StoreAsset(CoreAsset::Asset* asset, CoreAsset::AssetMetaData* assetMetaDat,
			const std::string& path);





	private:
		Storer();

	private:
		TextureStorer* mTextureStorer;



	};

}
