#include "Effect/Effect.h"
#include"Effect/RenderPassTwo.h"
#include"Parser/JsonParser.h"


//Quad::Effect::Effect(const std::string& name)
//	:mName(name)
//{
//
//}
//
//


Quad::Effect::Effect()
{
}

Quad::Effect::Effect(const Effect& effect)
{
	mName = effect.GetName();
	const std::vector<RenderPassTwo*> & renderPassVector = effect.GetStaticRenderPassVector();
	const std::vector<RenderPassTwo*> & renderPassVectorS = effect.GetSkinningRenderPassVector();

	mStaticMeshRenderPassVector.resize(renderPassVector.size());
	for (int i = 0; i < renderPassVector.size(); ++i)
	{
		RenderPassTwo* renderPass = new RenderPassTwo(*renderPassVector[i]);
		mStaticMeshRenderPassVector[i] = renderPass;
	}

	mSkinningMeshRenderPassVector.resize(renderPassVectorS.size());
	for (int i = 0; i < renderPassVectorS.size(); ++i)
	{
		RenderPassTwo* renderPass = new RenderPassTwo(*renderPassVectorS[i]);
		mSkinningMeshRenderPassVector[i] = renderPass;
	}


}

const std::vector<Quad::RenderPassTwo*> & Quad::Effect::GetStaticRenderPassVector() const
{
	return mStaticMeshRenderPassVector;
}

const std::vector<Quad::RenderPassTwo*>& Quad::Effect::GetSkinningRenderPassVector() const
{
	return mSkinningMeshRenderPassVector;
	// TODO: 여기에 return 문을 삽입합니다.
}




const std::string& Quad::Effect::GetName() const
{
	return mName;
	// TODO: 여기에 return 문을 삽입합니다.
}

unsigned long long Quad::Effect::GetUniqueID() const
{
	return mID;
}

void Quad::Effect::Serialize()
{
	JsonParser::Write("Effect_Name", mName);
	JsonParser::Write("Effect_ID", GetUniqueID());
	

}

void Quad::Effect::DeSerialize()
{
	JsonParser::Read("Effect_Name", mName);
	JsonParser::Read("Effect_ID", mID);


}


void Quad::Effect::SetStaticMeshRenderPassVector(std::vector<RenderPassTwo*>&& renderPassVector)
{
	mStaticMeshRenderPassVector = std::move(renderPassVector);
}

void Quad::Effect::SetSkinningMeshRenderPassVector(std::vector<RenderPassTwo*>&& renderPassVector)
{
	mSkinningMeshRenderPassVector = std::move(renderPassVector);

}

void Quad::Effect::SetUniqueID(unsigned long long id)
{
	mID = id;
}




