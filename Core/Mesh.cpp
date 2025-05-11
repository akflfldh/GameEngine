#include "Asset/Mesh/Mesh.h"
#include"Buffer/UploadBuffer.h"
#include"Parser/JsonParser.h"

namespace Quad
{



	Mesh::Mesh(const std::string& name , EMeshType meshTyp )
		:Asset(name,EAssetType::eMesh),mMeshType(meshTyp)
	{
	}

	Mesh::~Mesh()
	{


	}

	void Mesh::SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer)
	{
		mVertexBuffer = vertexBuffer;
	}

	void Mesh::SetIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer)
	{
		mIndexBuffer = indexBuffer;
	}

	//void Mesh::SetVertexVector(std::vector<StaticVertex>&& vertexVector)
	//{
	//	mVertexVector = std::move(vertexVector);
	//}

	void Mesh::SetIndexVector(std::vector<MeshIndexType>&& indexVector)
	{
		mIndexVector = std::move(indexVector);
		mIndexNum = mIndexVector.size();

	}

	void Mesh::SetSubMeshVector(std::vector<SubMesh>&& subMeshVector)
	{
		mSubMeshVector = std::move(subMeshVector);
	}

	void Mesh::SetVertexUploadBuffer(UploadBuffer* uploadBuffer)
	{
		mVertexUploadBuffer = uploadBuffer;
		SetVertexBuffer(uploadBuffer->GetResource());
	}

	UploadBuffer* Mesh::GetVertexUploadBuffer() const
	{
		return mVertexUploadBuffer;
	}




	const std::vector<SubMesh>& Mesh::GetSubMeshVector() const
	{
		return mSubMeshVector;
		// TODO: 여기에 return 문을 삽입합니다.
	}

	//const std::vector<StaticVertex> Mesh::GetVertexVector() const
	//{
	//	return mVertexVector;
	//}

	const std::vector<MeshIndexType> Mesh::GetIndexVector() const
	{
		return mIndexVector;
	}

	D3D12_GPU_VIRTUAL_ADDRESS Mesh::GetVertexBufferGpuVirtualAddress()
	{
		return mVertexBuffer->GetGPUVirtualAddress();
	}

	D3D12_GPU_VIRTUAL_ADDRESS Mesh::GetIndexBufferGpuVirtualAddress()
	{
		return mIndexBuffer->GetGPUVirtualAddress();
	}


	D3D12_INDEX_BUFFER_VIEW Mesh::GetIndexBufferView() const
	{

		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
		ibv.Format = MeshIndexDxgiType;
		ibv.SizeInBytes = sizeof(mIndexVector[0]) * mIndexNum;

		return ibv;

	}



	void Mesh::SetMeshType(EMeshType meshType)
	{
		mMeshType = meshType;
	}

	EMeshType Mesh::GetMeshType() const
	{
		return mMeshType;
	}

	void Mesh::SetMinMaxPoint(const DirectX::XMFLOAT3& minPoint, const DirectX::XMFLOAT3& maxPoint)
	{
		mMinPoint = minPoint;
		mMaxPoint = maxPoint;
	}

	std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> Mesh::GetMinMaxPoint() const
	{
		return {mMinPoint,mMaxPoint};
	}

	void Mesh::SetIndexNum(UINT num)
	{
		mIndexNum = num;
	}

	void Mesh::SetVertexNum(UINT num)
	{
		mVertexNum = num;
	}

	UINT Mesh::GetIndexNum() const
	{
		return mIndexNum;
	}

	UINT Mesh::GetVertexNum() const
	{
		return mVertexNum;
	}

	void Mesh::Serialize(const std::string& tag )
	{
		Asset::Serialize();
		JsonParser::Write("Mesh_VertexNum", mVertexNum);
		JsonParser::Write("Mesh_IndexNum", mIndexNum);
		JsonParser::Write("Mesh_MinPoint", mMinPoint);
		JsonParser::Write("Mesh_MaxPoint", mMaxPoint);
		JsonParser::Write("Mesh_Type", (int)mMeshType);
	}

	void Mesh::DeSerialize(const std::string& tag )
	{
		Asset::DeSerialize();
		JsonParser::Read("Mesh_VertexNum", mVertexNum);
		JsonParser::Read("Mesh_IndexNum", mIndexNum);
		JsonParser::Read("Mesh_MinPoint", mMinPoint);
		JsonParser::Read("Mesh_MaxPoint", mMaxPoint);
		//int meshType = 0;
		//JsonParser::Read("Mesh_Type", meshType);
	//	mMeshType = (EMeshType)meshType;
	}

	bool Mesh::CheckType(const ContentItem* source)
	{

		bool ret = Asset::CheckType(source);
		if (!ret)
			return false;

		const Asset* asset = static_cast<const Asset*>(source);
		if (asset->GetAssetType() != Quad::EAssetType::eMesh)
			return false;

		return true;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> Mesh::GetVertexBuffer() const
	{
		return mVertexBuffer;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> Mesh::GetIndexBuffer() const
	{
		return mIndexBuffer;
	}

	bool Mesh::InnerCopy(const ContentItem* source)
	{
	
		Asset::InnerCopy(source);


		const Mesh* mesh = static_cast<const Mesh*>(source);


		SetVertexBuffer(mesh->GetVertexBuffer());
		SetIndexBuffer(mesh->GetIndexBuffer());
		
		mIndexVector = mesh->mIndexVector;
		mSubMeshVector = mesh->mSubMeshVector;

		mIndexNum = mesh->mIndexNum;
		mVertexNum = mesh->mVertexNum;


		mMinPoint = mesh->mMinPoint;
		mMaxPoint = mesh->mMaxPoint;
		mMeshType = mesh->mMeshType;


		return true;
	}

	/*void Mesh::SetID(size_t id)
	{
		mID = id;
	}

	size_t Mesh::GetID() const
	{
		return mID;
	}*/



	void CaculateStaticMeshNormalVector(std::vector<StaticVertex>& vertexVector, const std::vector<MeshIndexType>& indexVector)
	{

		MeshIndexType index = 0;
		while (index < indexVector.size())
		{

			MeshIndexType vertexOneIndex = indexVector[index];
			MeshIndexType vertexTwoIndex = indexVector[index + 1];
			MeshIndexType vertexThreeIndex = indexVector[index + 2];

			StaticVertex& vertexOne = vertexVector[vertexOneIndex];
			StaticVertex& vertexTwo = vertexVector[vertexTwoIndex];
			StaticVertex& vertexThree = vertexVector[vertexThreeIndex];

			//이 삼각형의 노멀을 구하고 이 정점들의 노멀값은 정규화하고 노멀값에 더한다.
			//그리고 다 처리하고나서 다시 정규화한다.

			//v0 -v1 = e1  , v0 v2  =e2
			DirectX::XMFLOAT3& vertexOnePos = vertexOne.mPos;
			DirectX::XMFLOAT3& vertexTwoPos = vertexTwo.mPos;
			DirectX::XMFLOAT3& vertexThreePos = vertexThree.mPos;

			DirectX::XMVECTOR e1;
			DirectX::XMVECTOR e2;

			e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&vertexTwoPos), DirectX::XMLoadFloat3(&vertexOnePos));
			e2 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&vertexThreePos), DirectX::XMLoadFloat3(&vertexOnePos));

			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e1, e2)));



			vertexOne.mNormal.x += normal.x;
			vertexOne.mNormal.y += normal.y;
			vertexOne.mNormal.z += normal.z;
			vertexTwo.mNormal.x += normal.x;
			vertexTwo.mNormal.y += normal.y;
			vertexTwo.mNormal.z += normal.z;
			vertexThree.mNormal.x += normal.x;
			vertexThree.mNormal.y += normal.y;
			vertexThree.mNormal.z += normal.z;

			index += 3;

		}

		for (int i = 0; i < vertexVector.size(); ++i)
		{
			DirectX::XMStoreFloat3(&vertexVector[i].mNormal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vertexVector[i].mNormal)));
		}


	}

	void CaculateSkinningMeshNormalVector(std::vector<SkinningVertex>& vertexVector, const std::vector<MeshIndexType>& indexVector)
	{
		MeshIndexType index = 0;
		while (index < indexVector.size())
		{

			MeshIndexType vertexOneIndex = indexVector[index];
			MeshIndexType vertexTwoIndex = indexVector[index + 1];
			MeshIndexType vertexThreeIndex = indexVector[index + 2];

			SkinningVertex& vertexOne = vertexVector[vertexOneIndex];
			SkinningVertex& vertexTwo = vertexVector[vertexTwoIndex];
			SkinningVertex& vertexThree = vertexVector[vertexThreeIndex];

			//이 삼각형의 노멀을 구하고 이 정점들의 노멀값은 정규화하고 노멀값에 더한다.
			//그리고 다 처리하고나서 다시 정규화한다.

			//v0 -v1 = e1  , v0 v2  =e2
			DirectX::XMFLOAT3& vertexOnePos = vertexOne.mPos;
			DirectX::XMFLOAT3& vertexTwoPos = vertexTwo.mPos;
			DirectX::XMFLOAT3& vertexThreePos = vertexThree.mPos;

			DirectX::XMVECTOR e1;
			DirectX::XMVECTOR e2;

			e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&vertexTwoPos), DirectX::XMLoadFloat3(&vertexOnePos));
			e2 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&vertexThreePos), DirectX::XMLoadFloat3(&vertexOnePos));

			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e1, e2)));



			vertexOne.mNormal.x += normal.x;
			vertexOne.mNormal.y += normal.y;
			vertexOne.mNormal.z += normal.z;
			vertexTwo.mNormal.x += normal.x;
			vertexTwo.mNormal.y += normal.y;
			vertexTwo.mNormal.z += normal.z;
			vertexThree.mNormal.x += normal.x;
			vertexThree.mNormal.y += normal.y;
			vertexThree.mNormal.z += normal.z;

			index += 3;

		}

		for (int i = 0; i < vertexVector.size(); ++i)
		{
			DirectX::XMStoreFloat3(&vertexVector[i].mNormal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vertexVector[i].mNormal)));
		}
	}


}