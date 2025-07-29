#pragma once


#include<string>

namespace Import { class TextureImporter; }
namespace CoreAsset {
	class Texture;
	class TextureManager;
	class AssetMetaDataManager;
}


namespace Quad
{
	class EditorTextureImporter
	{
	public:
		static EditorTextureImporter* GetInstance();

		
		EditorTextureImporter(Import::TextureImporter* textureImporterCore, CoreAsset::TextureManager * textureManager,
			CoreAsset::AssetMetaDataManager * assetMetaDataManager);
		~EditorTextureImporter();

		CoreAsset::Texture* Import(const std::string& filePath, const std::string& logicalPath) const;

	private:
		bool ReigsterTextureMetaData(CoreAsset::Texture* texture, const std::string & filePath , const std::string & logicalPath) const;

	private:
		static EditorTextureImporter* mInstance;
		Import::TextureImporter* mTextureImporterCore;
		CoreAsset::TextureManager* mTextureManager;
		CoreAsset::AssetMetaDataManager* mAssetMetaDataManager;

	};

}
