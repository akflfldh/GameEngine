#include "GeoGenerator.h"
namespace Quad
{

	bool GeometryGenerator::CreateRawRectMeshData(RawStaticMeshData* oRawMeshData)
	{


		std::vector<StaticVertex> mVertexVector(4);
		std::vector<MeshIndexType> mIndexVector(6);
		std::vector<RawSubMeshData> mRawSubMeshVector(1);
		std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> boundingBox;

		mVertexVector[0].mPos = { -0.5f,0.5f,0.0f };
		mVertexVector[0].mTex = { 0.0f,0.0f };

		mVertexVector[1].mPos = { 0.5f,0.5f,0.0f };
		mVertexVector[1].mTex = { 1.0f,0.0f };

		mVertexVector[2].mPos = { -0.5f,-0.5f,0.0f };
		mVertexVector[2].mTex = { 0.0f,1.0f };

		mVertexVector[3].mPos = { 0.5f,-0.5f,0.0f };
		mVertexVector[3].mTex = { 1.0f,1.0f };

		boundingBox.first = boundingBox.second = mVertexVector[0].mPos;

		for (int i = 1; i < mVertexVector.size(); ++i)
		{

			//max
			if (mVertexVector[i].mPos.x > boundingBox.second.x)
				boundingBox.second.x = mVertexVector[i].mPos.x;

			if (mVertexVector[i].mPos.y > boundingBox.second.y)
				boundingBox.second.y = mVertexVector[i].mPos.y;

			/*if (mVertexVector[i].mPos.z > boundingBox.second.z)
				boundingBox.second.z =  mVertexVector[i].mPos.z;*/

			boundingBox.second.z = 0.5f;

			//min
			if (mVertexVector[i].mPos.x < boundingBox.first.x)
				boundingBox.first.x = mVertexVector[i].mPos.x;

			if (mVertexVector[i].mPos.y < boundingBox.first.y)
				boundingBox.first.y = mVertexVector[i].mPos.y;

			boundingBox.first.z = -0.5f;

			/*if (mVertexVector[i].mPos.z < boundingBox.first.z)
				boundingBox.first.z = mVertexVector[i].mPos.z;*/

		}




		//surface1
		mIndexVector[0] = 0;
		mIndexVector[1] = 1;
		mIndexVector[2] = 2;

		//surface2
		mIndexVector[3] = 2;
		mIndexVector[4] = 1;
		mIndexVector[5] = 3;


		mRawSubMeshVector[0].mVertexOffset = 0;
		mRawSubMeshVector[0].mIndexFirst = 0;
		mRawSubMeshVector[0].mIndexLast = 6;
		mRawSubMeshVector[0].mName = "Rectangle";
		mRawSubMeshVector[0].mMaterialName = "Default";


		oRawMeshData->mVertexVector = std::move(mVertexVector);
		oRawMeshData->mIndexVector = std::move(mIndexVector);
		oRawMeshData->mRawSubMeshVector = std::move(mRawSubMeshVector);
		oRawMeshData->mBoundingBox = boundingBox;
		oRawMeshData->mName = "Rectangle";
		oRawMeshData->mMeshType = EMeshType::eStaticMesh;
		return true;
	}



	bool GeometryGenerator::CreateRawBoundaryRectMeshData(RawStaticMeshData* oRawMeshData)
	{


		std::vector<StaticVertex> mVertexVector(16);
		std::vector<MeshIndexType> mIndexVector(24);
		std::vector<RawSubMeshData> mRawSubMeshVector(4);
		std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> boundingBox;

		float margin = 0.02f;
		//top rect1
		mVertexVector[0].mPos = { -0.5f - margin,0.5f + margin,0.0f };
		mVertexVector[0].mTex = { 0.0f,0.0f };

		mVertexVector[1].mPos = { 0.5f + margin,0.5f + margin,0.0f };
		mVertexVector[1].mTex = { 1.0f,0.0f };

		mVertexVector[2].mPos = { -0.5f - margin,0.5f - margin,0.0f };
		mVertexVector[2].mTex = { 0.0f,1.0f };

		mVertexVector[3].mPos = { 0.5f + margin,0.5f - margin,0.0f };
		mVertexVector[3].mTex = { 1.0f,1.0f };
		//bottom rect2
		mVertexVector[4].mPos = { -0.5f - margin,-0.5f + margin,0.0f };
		mVertexVector[4].mTex = { 0.0f,0.0f };

		mVertexVector[5].mPos = { 0.5f + margin,-0.5f + margin,0.0f };
		mVertexVector[5].mTex = { 1.0f,0.0f };

		mVertexVector[6].mPos = { -0.5f - margin, -0.5f - margin,0.0f };
		mVertexVector[6].mTex = { 0.0f,1.0f };

		mVertexVector[7].mPos = { 0.5f + margin, -0.5f - margin,0.0f };
		mVertexVector[7].mTex = { 1.0f,1.0f };

		//left rect3
		mVertexVector[8].mPos = { -0.5f - margin,0.5f + margin,0.0f };
		mVertexVector[8].mTex = { 0.0f,0.0f };

		mVertexVector[9].mPos = { -0.5f + margin,0.5f + margin,0.0f };
		mVertexVector[9].mTex = { 0.0f,1.0f };

		mVertexVector[10].mPos = { -0.5f - margin, -0.5f - margin,0.0f };
		mVertexVector[10].mTex = { 1.0f,0.0f };

		mVertexVector[11].mPos = { -0.5f + margin, -0.5f - margin,0.0f };
		mVertexVector[11].mTex = { 1.0f,1.0f };

		//right rect4
		mVertexVector[12].mPos = { 0.5f - margin,0.5f + margin,0.0f };
		mVertexVector[12].mTex = { 0.0f,0.0f };

		mVertexVector[13].mPos = { 0.5f + margin,0.5f + margin,0.0f };
		mVertexVector[13].mTex = { 0.0f,1.0f };

		mVertexVector[14].mPos = { 0.5f - margin, -0.5f - margin,0.0f };
		mVertexVector[14].mTex = { 1.0f,0.0f };

		mVertexVector[15].mPos = { 0.5f + margin, -0.5f - margin,0.0f };
		mVertexVector[15].mTex = { 1.0f,1.0f };




		boundingBox.first = boundingBox.second = mVertexVector[0].mPos;

		for (int subMeshIndex = 0; subMeshIndex < mRawSubMeshVector.size(); ++subMeshIndex)
		{
			std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> boundingBox;
			boundingBox.first = boundingBox.second = mVertexVector[subMeshIndex * 4].mPos;

			for (int i = subMeshIndex * 4 + 1; i < (subMeshIndex + 1) * 4; ++i)
			{

				//max
				if (mVertexVector[i].mPos.x > boundingBox.second.x)
					boundingBox.second.x = mVertexVector[i].mPos.x;

				if (mVertexVector[i].mPos.y > boundingBox.second.y)
					boundingBox.second.y = mVertexVector[i].mPos.y;

				/*if (mVertexVector[i].mPos.z > boundingBox.second.z)
					boundingBox.second.z =  mVertexVector[i].mPos.z;*/

				boundingBox.second.z = 0.5f;

				//min
				if (mVertexVector[i].mPos.x < boundingBox.first.x)
					boundingBox.first.x = mVertexVector[i].mPos.x;

				if (mVertexVector[i].mPos.y < boundingBox.first.y)
					boundingBox.first.y = mVertexVector[i].mPos.y;

				boundingBox.first.z = -0.5f;

				/*if (mVertexVector[i].mPos.z < boundingBox.first.z)
					boundingBox.first.z = mVertexVector[i].mPos.z;*/

			}

			mRawSubMeshVector[subMeshIndex].mBoundingBox = boundingBox;
		}


		//top rect1
		//surface1
		mIndexVector[0] = 0;
		mIndexVector[1] = 1;
		mIndexVector[2] = 2;

		//surface2
		mIndexVector[3] = 1;
		mIndexVector[4] = 3;
		mIndexVector[5] = 2;

		//bottom rect2
		//surface1
		mIndexVector[6] = 4;
		mIndexVector[7] = 5;
		mIndexVector[8] = 6;

		//surface2
		mIndexVector[9] = 5;
		mIndexVector[10] = 7;
		mIndexVector[11] = 6;


		//left rect3
		//surface1
		mIndexVector[12] = 8;
		mIndexVector[13] = 9;
		mIndexVector[14] = 11;

		//surface2
		mIndexVector[15] = 8;
		mIndexVector[16] = 11;
		mIndexVector[17] = 10;

		//right rect4
				//surface1
		mIndexVector[18] = 12;
		mIndexVector[19] = 13;
		mIndexVector[20] = 15;

		//surface2
		mIndexVector[21] = 12;
		mIndexVector[22] = 15;
		mIndexVector[23] = 14;


		for (int i = 0; i < 4; ++i)
		{

			mRawSubMeshVector[i].mVertexOffset = i * 4;
			mRawSubMeshVector[i].mIndexFirst = i * 6;
			mRawSubMeshVector[i].mIndexLast = (i + 1) * 6;
			mRawSubMeshVector[i].mName = "Rectangle_" + std::to_string(i);
			mRawSubMeshVector[i].mMaterialName = "default.mat";

		}

		oRawMeshData->mVertexVector = std::move(mVertexVector);
		oRawMeshData->mIndexVector = std::move(mIndexVector);
		oRawMeshData->mRawSubMeshVector = std::move(mRawSubMeshVector);
		//oRawMeshData.mBoundingBox = boundingBox;

		return true;














		return false;
	}
}