#pragma once

#include"header.h"
#include"Predeclare.h"
#include"Asset/Material/Material.h"
#include"Parser/JsonParser.h"
#include"ResourceSmartPointer/MaterialSmartPointer.h"
#include"Asset/Material/ModelMaterial/ModelMaterial.h"
#include"Asset/Material/LineBaseMaterial.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	enum class EMeshType:int
	{
		eStaticMesh = 0,
		eSknningMesh,
		eLine
	};


	struct StaticVertex
	{

		DirectX::XMFLOAT3 mPos;
		DirectX::XMFLOAT3 mNormal;
		DirectX::XMFLOAT2 mTex;

		bool operator==(const Quad::StaticVertex& vertex) const
		{
			DirectX::XMFLOAT3 ret;
			DirectX::XMStoreFloat3(&ret, DirectX::XMVectorEqual(DirectX::XMLoadFloat3(&mPos), DirectX::XMLoadFloat3(&vertex.mPos)));
			if ((ret.x+ ret.y+ ret.z)!=3)
				return false;

			DirectX::XMStoreFloat3(&ret, DirectX::XMVectorEqual(DirectX::XMLoadFloat3(&mNormal), DirectX::XMLoadFloat3(&vertex.mNormal)));
			if ((ret.x + ret.y + ret.z) != 3)
				return false;

		//	DirectX::XMFLOAT2 ret2;
			DirectX::XMStoreFloat3(&ret, DirectX::XMVectorEqual(DirectX::XMLoadFloat2(&mTex), DirectX::XMLoadFloat2(&vertex.mTex)));
			if ((ret.x + ret.y ) != 2)
				return false;


			return true;

		}


	};

	struct SkinningVertex
	{
		DirectX::XMFLOAT3 mPos;
		DirectX::XMFLOAT3 mNormal;
		DirectX::XMFLOAT2 mTex;


		DirectX::XMFLOAT4 mBoneWeight[2];
		DirectX::XMINT4  mBoneIndex[2];
	




		bool operator==(const Quad::SkinningVertex& vertex) const
		{
			DirectX::XMFLOAT3 ret;
			DirectX::XMStoreFloat3(&ret, DirectX::XMVectorEqual(DirectX::XMLoadFloat3(&mPos), DirectX::XMLoadFloat3(&vertex.mPos)));
			if ((ret.x + ret.y + ret.z) != 3)
				return false;

			DirectX::XMStoreFloat3(&ret, DirectX::XMVectorEqual(DirectX::XMLoadFloat3(&mNormal), DirectX::XMLoadFloat3(&vertex.mNormal)));
			if ((ret.x + ret.y + ret.z) != 3)
				return false;

			//DirectX::XMFLOAT2 ret2;
			DirectX::XMStoreFloat3(&ret, DirectX::XMVectorEqual(DirectX::XMLoadFloat2(&mTex), DirectX::XMLoadFloat2(&vertex.mTex)));
			if ((ret.x + ret.y + ret.z) != 2)
				return false;

			


			return true;

		}


















	};


	using MeshIndexType = uint32_t;


#define MeshIndexDxgiType DXGI_FORMAT_R32_UINT;


	struct SubMesh
	{
		std::string mName;
		std::pair<MeshIndexType, MeshIndexType> mIndexRange = { 0,0 };//end 반개방범위
		size_t mVertexOffset = 0;
		size_t mVertexNum = 0;

		Mesh* mMesh = nullptr;
		MaterialSmartPointer mMaterialPointer = nullptr;
		

	//	BoundingBox* mBoundingBox = nullptr;
		size_t mID;

		//원래라면 subMesh가 파괴될떄 이 subMesh만을 위한 material을 해제해야한다.

		void Serialize()
		{
			JsonParser::Write("SubMesh_Name", mName);
			JsonParser::Write("SubMesh_IndexRangeFirst", mIndexRange.first);
			JsonParser::Write("SubMesh_IndexRangeSecond", mIndexRange.second);
			JsonParser::Write("SubMesh_VertexOffset", mVertexOffset);
			JsonParser::Write("SubMesh_VertexNum", mVertexNum);

			//JsonParser::Write("SubMesh_MaterialID,mMaterial);
			JsonParser::Write("SubMesh_ID", mID);
		};


		void DeSerialize()
		{
			
			JsonParser::Read("SubMesh_Name", mName);
			JsonParser::Read("SubMesh_IndexRangeFirst", mIndexRange.first);
			JsonParser::Read("SubMesh_IndexRagneSecond", mIndexRange.second);
			JsonParser::Read("SubMesh_VertexOffset", mVertexOffset);
			JsonParser::Read("SubMesh_VertexNum", mVertexNum);

			JsonParser::Read("SubMesh_ID", mID);

		};

		SubMesh() = default;
		~SubMesh() = default;
		
		SubMesh(const SubMesh& subMesh)
			:mName(subMesh.mName), mIndexRange(subMesh.mIndexRange), mVertexOffset(subMesh.mVertexOffset),
			mVertexNum(subMesh.mVertexNum), mMesh(subMesh.mMesh), mMaterialPointer(subMesh.mMaterialPointer), mID(subMesh.mID)
		{
			

		}

		SubMesh(SubMesh&& subMesh)noexcept
			:mName(subMesh.mName), mIndexRange(subMesh.mIndexRange), mVertexOffset(subMesh.mVertexOffset),
			mVertexNum(subMesh.mVertexNum), mMesh(subMesh.mMesh), mMaterialPointer(subMesh.mMaterialPointer), mID(subMesh.mID)
		{
			


		}


		SubMesh& operator=(const SubMesh& subMesh)
		{
			mName = subMesh.mName;
			mIndexRange = subMesh.mIndexRange;
			mVertexOffset = subMesh.mVertexOffset;
			mVertexNum = subMesh.mVertexNum;
			mMesh = subMesh.mMesh;
			mMaterialPointer = subMesh.mMaterialPointer;
			mID = subMesh.mID;

			return *this;

		}
		SubMesh& operator=(SubMesh&& subMesh) noexcept
		{
			mName = subMesh.mName;
			mIndexRange = subMesh.mIndexRange;
			mVertexOffset = subMesh.mVertexOffset;
			mVertexNum = subMesh.mVertexNum;
			mMesh = subMesh.mMesh;
			mMaterialPointer = subMesh.mMaterialPointer;
			mID = subMesh.mID;

			return *this;
		}



	};

	
	//model submesh는 자원리소스들에 대한것이아니라
	//오브젝트들이 고유하게가지는 submesh
	struct BaseModelSubMesh
	{
		std::string mName;
		std::pair<MeshIndexType, MeshIndexType> mIndexRange = { 0,0 };//end 반개방범위
		size_t mVertexOffset = 0;
		size_t mVertexNum = 0;


	};

	struct LineModelSubMesh :public BaseModelSubMesh
	{
		LineBaseMaterial mModelMaterial;

	};


	//엔티티 인스턴스별 메시 
	struct ModelSubMesh :public BaseModelSubMesh
	{
		//std::string mName;
		//std::pair<MeshIndexType, MeshIndexType> mIndexRange = { 0,0 };//end 반개방범위
		//size_t mVertexOffset = 0;
		//size_t mVertexNum = 0;
		Mesh* mMesh = nullptr;
		ModelMaterial mModelMaterial;
		

		ModelSubMesh& operator=(const SubMesh& subMesh)
		{
			mName = subMesh.mName;
			mIndexRange = subMesh.mIndexRange;
			mVertexOffset = subMesh.mVertexOffset;
			mVertexNum = subMesh.mVertexNum;
			mMesh = subMesh.mMesh;
			mModelMaterial = *(subMesh.mMaterialPointer);

			return *this;
		}

		void Serialize(const std::string & tag)
		{

			mModelMaterial.Serialize(tag);

		}


		void DeSerialize(const std::string& tag)
		{
			mModelMaterial.DeSerialize(tag);

		}



	};








}
