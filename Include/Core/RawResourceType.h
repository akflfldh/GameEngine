#pragma once



#include<vector>
#include<string>
#include"Asset/Mesh/MeshType.h"
#include"Asset/Material/Material.h"
//#include"Attribute.h"

namespace Quad
{
	
	//raw들은 이미 나만의 파일형식인 파일들을 로드할때 사용하는것, 일반적이obj,fbx는 따로 파싱을 먼저 해야한다.
	struct RawSubMeshData
	{
		size_t mVertexOffset;
		MeshIndexType mIndexFirst, mIndexLast;

		std::string mName;
		std::string mMaterialName;
		std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> mBoundingBox;


		/*bool operator==(const RawSubMeshData& rawSubMeshData) const
		{
			if (mName != rawSubMeshData.mName || mMaterialName != rawSubMeshData.mMaterialName)
			{
				return false;
			}
			return true;
		}*/

	};












	struct RawMeshData
	{
		RawMeshData(EMeshType type)
			:mMeshType(type)
		{

		}
		virtual ~RawMeshData() =0 {};

		std::string mName;
		std::vector<MeshIndexType> mIndexVector;
		std::vector<RawSubMeshData> mRawSubMeshVector;
		std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> mBoundingBox;
		EMeshType mMeshType= EMeshType::eStaticMesh;
	};

	struct RawStaticMeshData:public RawMeshData
	{
		RawStaticMeshData()
			:RawMeshData(EMeshType::eStaticMesh)
		{

		}
		~RawStaticMeshData()=default;

		std::vector<StaticVertex> mVertexVector;
	};

	struct RawSkinningMeshData :public RawMeshData
	{
		RawSkinningMeshData()
			:RawMeshData(EMeshType::eSknningMesh)
		{

		}
		~RawSkinningMeshData() = default;
		std::vector<SkinningVertex> mVertexVector;
	};

	//struct RawLineMeshData :public RawMeshData
	//{
	//	RawLineMeshData()
	//		:RawMeshData(EMeshType::eLine)
	//	{
	//	}
	//	~RawLineMeshData() = default;

	//	std::vector<LineVertex> mVertexVector;
	//};

	struct RawMaterialData
	{
		std::string mName;
		std::string mDiffuseMapName;
		std::string mNormalMapName;


		MaterialType mType;
		DirectX::XMFLOAT3 mSpecular;
		DirectX::XMFLOAT3 mAmbient;
		float mShiness =0.0f;
		std::string mEffectName="Default.effect";


		//true이면 머터리얼이없어서 디폴트 머터리얼이 적용되었다라는의미
		bool mDefaultFlag = false;
		

	};

	struct RawTextureData
	{
		std::string mFilePath;


	};



	struct RawSchemaData
	{
		std::string mType;
		std::string mClassId;

		DirectX::XMFLOAT3 mPos;
		DirectX::XMFLOAT3 mScale;
		DirectX::XMFLOAT4 mRotate;

		std::string mBoundingVolume;
		std::string mMesh;

		std::map<std::string, Attribute> mAttributeTable;
	};





	struct RawShaderDataTwo
	{
		std::string mFile;
		std::string mEntryPoint;
		std::string mVersion;
	};

	struct RawRenderPassDataTwo
	{
		RawShaderDataTwo mVertexShaderData;
		RawShaderDataTwo mPixelShaderData;
		RawShaderDataTwo mGeometryShaderData;


		//그래픽파이프라인의 옵션설정
		//			<key : value> 
		std::vector<std::pair<std::string, std::string>> mGraphicsPipelineCfgVector;

	};

	struct RawEffectDataTwo
	{
		std::string mEffectName;
		std::vector<RawRenderPassDataTwo> mRenderPassDataVector;
	};



}