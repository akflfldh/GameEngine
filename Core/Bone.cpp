#include"Asset/Animation/Bone.h"

Quad::Bone::Bone()
	:mName("")
{
}

void Quad::Bone::SetName(const std::string& name)
{
	mName = name;
}

const std::string& Quad::Bone::GetName() const
{
	return mName;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::Bone::SetBoneType(Quad::EBoneType type)
{
	mBoneType = type;
}

Quad::EBoneType Quad::Bone::GetBoneType() const
{
	return mBoneType;
}

void Quad::Bone::Serialize(const std::string& tag)
{



}

void Quad::Bone::DeSerialize(const std::string& tag)
{




}
