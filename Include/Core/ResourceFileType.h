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

		DirectX::XMFLOAT4 mDiffuse;
		DirectX::XMFLOAT3 mFresnelR0;
		DirectX::XMFLOAT3 mSpecular;
		DirectX::XMFLOAT3 mAmbient;
		DirectX::XMFLOAT3 mColor;
		DirectX::XMFLOAT3 mColorIntensity;
		DirectX::XMFLOAT3 mEmissive;
		unsigned long long  mDiffuseMapID;
		unsigned long long mNormalMapID;
		unsigned long long mEffectID;
		float mShiness = 0.2f;
	//	std::string mEffectName = "Default.effect";
		DirectX::XMFLOAT4X4 mTextureTransformMatrix[3]; //배열로 여러개존재하게할수도있을거같은데

	};
#pragma pack(pop) 





	





}