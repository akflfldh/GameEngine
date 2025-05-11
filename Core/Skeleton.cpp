#include "Asset/Skeleton/Skeleton.h"
#include"Asset/Animation/Bone.h"
#include"Utility.h"


#include"Parser/JsonParser.h"
#include"Core/BoneFactory.h"
Quad::Skeleton::Skeleton(const std::string& name)
	:Asset(name,EAssetType::eSkeleton)
{
}
Quad::Skeleton::~Skeleton()
{
	for (auto bone : mBoneVector)
	{
		BoneFactory::ReleaseBone(bone);
	}


}
void Quad::Skeleton::AddBone(Bone* bone, int parentBoneIndex)
{
	mBoneHiererachyVector.push_back(parentBoneIndex);
	mBoneVector.push_back(bone);
	mOffsetMatrixVector.push_back(Utility::GetIdentityMatrixF());
}

const std::vector<Quad::Bone*>& Quad::Skeleton::GetBoneVector() const
{
	return mBoneVector;
	// TODO: 여기에 return 문을 삽입합니다.
}


void Quad::Skeleton::Copy(Quad::Skeleton && skeleton)
{
	mBoneVector = std::move(skeleton.mBoneVector);
	mBoneHiererachyVector = std::move(skeleton.mBoneHiererachyVector);
	mOffsetMatrixVector = std::move(skeleton.mOffsetMatrixVector);



}





//std::vector<Quad::Bone*>& Quad::Skeleton::GetBoneVector()
//{
//	return mBoneVector;
//	// TODO: 여기에 return 문을 삽입합니다.
//}

const std::vector<int>& Quad::Skeleton::GetBoneHiererachyVector() const
{
	return mBoneHiererachyVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

int Quad::Skeleton::GetBoneID(const std::string& name) const
{

	for (int index = 0; index < mBoneVector.size(); ++index)
	{
		if (mBoneVector[index]->GetName() == name)
			return index;
	}

	//assert
	return -1;

}

Quad::Bone* Quad::Skeleton::GetBone(const std::string& name) const
{
	for (int index = 0; index < mBoneVector.size(); ++index)
	{
		if (mBoneVector[index]->GetName() == name)
			return mBoneVector[index];
	}

	return nullptr;
}

Quad::Bone* Quad::Skeleton::GetBone(int boneID) const
{
	return mBoneVector[boneID];
}

void Quad::Skeleton::SetOffsetMatrix(int boneID, const DirectX::XMFLOAT4X4& matrix)
{
	mOffsetMatrixVector[boneID] = matrix;
}

const std::vector<DirectX::XMFLOAT4X4>& Quad::Skeleton::GetOffsetMatrixVector() const
{

	return mOffsetMatrixVector;
	// TODO: 여기에 return 문을 삽입합니다.
}



