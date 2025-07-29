#pragma once

#include"AssetType.h"
#include<string>


namespace QuadRW {
	class BinaryWriter;
	class BinaryReader;
}


namespace CoreAsset
{

	struct SerializedAsset
	{
		AssetID mAssetID;
		EAssetType mAssetType;
		std::string mAssetName;

		virtual void Serialize(QuadRW::BinaryWriter& writer);
		virtual void DeSerialize(QuadRW::BinaryReader& reader);
	};



	void SerializeTextureProperties(const TextureProperties& textureProperties, QuadRW::BinaryWriter& writer);
	void DeSerializeTextureProperties(TextureProperties& oTextureProperties, QuadRW::BinaryReader& reader);


	struct SerializedTexture : public SerializedAsset
	{
		TextureProperties mTextureProperties;
		GRM::ETextureUsage mTextureUsage;
		virtual void Serialize(QuadRW::BinaryWriter& writer) override;
		virtual void DeSerialize(QuadRW::BinaryReader& reader) override;
	};







	struct SerializedAssetRawData
	{

	};

	struct SerializedTextureRawData :public SerializedAssetRawData
	{
		GRM::TextureDesc mTextureRawData;
		void Serialize(QuadRW::BinaryWriter& writer);
		void DeSerialize(QuadRW::BinaryReader& reader);
		

	


	private:
		void SerializeImage(const GRM::Image& image, const uint8_t* mMemoryBase, QuadRW::BinaryWriter& writer);
		void SerializeScratchImage(const GRM::ScratchImage& scratchImage, QuadRW::BinaryWriter& writer);
		void SerializeTextureMetaData(const GRM::TexMetaData& texMetaData, QuadRW::BinaryWriter& writer);


		void DeSerializeImage( GRM::Image& oImage,  uint8_t* mMemoryBase, QuadRW::BinaryReader& reader);
		void DeSerializeScratchImage( GRM::ScratchImage& oScratchImage, QuadRW::BinaryReader& reader);
		void DeSerializeTextureMetaData( GRM::TexMetaData& oTexMetaData, QuadRW::BinaryReader& reader);
	};




	struct SerializedTextureRuntime :public SerializedTexture
	{
		SerializedTextureRawData mSerializedRawData;
		virtual void Serialize(QuadRW::BinaryWriter& writer) override;
	};









}