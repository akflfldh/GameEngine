#include "VertexHelper.h"

void Quad::VertexHelper::CaculateSkinningMeshNormalVector(std::vector<SkinningVertex>& vertexVector, const std::vector<MeshIndexType>& indexVector)
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

void Quad::VertexHelper::CaculateStaticMeshNormalVector(std::vector<StaticVertex>& vertexVector, const std::vector<MeshIndexType>& indexVector)
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
