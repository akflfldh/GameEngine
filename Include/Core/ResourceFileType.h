#pragma once


namespace Quad
{
	
	struct ResourcePackageHeader
	{




	};

	
	struct ResourcePackageEntryMeta
	{
		unsigned long long mNameLength;
		std::string mName;
		unsigned long long mID;
		unsigned long long mDataSize;
		unsigned long long mDataOffset;
	};

	struct ResourceMeshPackageEntryMeta
	{
		unsigned long long mNameLength;
		std::string mName;
		int mMeshType;
		unsigned long long mID;
		unsigned long long mDataSize;
		unsigned long long mDataOffset;
	};




#pragma pack(push, 1) 
	struct MaterialPackageRawData
	{
		MaterialType mType;

		DirectX::XMFLOAT4 mDiffuse = { 1.0f,1.0f,1.0f,1.0f };
		DirectX::XMFLOAT3 mFresnelR0 = { 0.1f,0.1f,0.1f };
		DirectX::XMFLOAT3 mSpecular = { 0.5f,0.5f,0.5f };
		DirectX::XMFLOAT3 mAmbient = { 0.2f,0.2f,0.2f };
		DirectX::XMFLOAT3 mColor = { 1.0f,1.0f,1.0f };
		DirectX::XMFLOAT3 mColorIntensity = { 1.0f,1.0f,1.0f };
		unsigned long long  mDiffuseMapID;
		unsigned long long mNormalMapID;
		unsigned long long mEffectID;
		float mShiness = 0.2f;
	//	std::string mEffectName = "Default.effect";
		DirectX::XMFLOAT4X4 mTextureTransformMatrix[3]; //배열로 여러개존재하게할수도있을거같은데

	};
#pragma pack(pop) 





	





}