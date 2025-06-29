#include "Effect/Effect.h"
#include"Effect/RenderPassTwo.h"
#include"Parser/JsonParser.h"

#include"ResourceManager/TextureManager/TextureManager.h"
#include"Shader/ShaderResource.h"

#include"Shader/ShaderResourceConstantBuffer.h"

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

Quad::Effect::~Effect()
{
	for (int i = 0; i < mStaticMeshRenderPassVector.size(); ++i)
	{

		delete	mStaticMeshRenderPassVector[i];

	}
	for (int i = 0; i < mSkinningMeshRenderPassVector.size(); ++i)
	{

		delete	mSkinningMeshRenderPassVector[i];

	}

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

void Quad::Effect::AddCustomTexture(Quad::ETextureType textureType,std::pair<float, float> resizeRate)
{
	mCustonTextureTypeVector.push_back(textureType);

	mCustomTextureNum++;
	mWindowResizeRateCustomTextureVector.push_back(resizeRate);

}

unsigned int Quad::Effect::GetCustomTextureNum() const
{
	return mCustomTextureNum;
}

const std::vector<std::pair<float, float>>& Quad::Effect::GetCustomTextureResizeRateVector() const
{
	return mWindowResizeRateCustomTextureVector;

	// TODO: 여기에 return 문을 삽입합니다.
}

const std::vector<unsigned long long>& Quad::Effect::GetCustomTextureIDVector() const
{
	return mCustomTextureIDVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

const std::vector<Quad::ETextureType>& Quad::Effect::GetCustomTextureTypeVector() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return mCustonTextureTypeVector;
}

void Quad::Effect::SetCustomTextureIDVector(std::vector<unsigned long long>&& customRenderTargetTextureIDVector)
{
	mCustomTextureIDVector = std::move(customRenderTargetTextureIDVector);

}


void Quad::Effect::SetCustomTextureVector(std::vector<Texture*>&& customTextureVector)
{
	mCustomTextureVector = std::move(customTextureVector);

}

const std::vector<Quad::Texture*>& Quad::Effect::GetCustomTextureVector() const
{

	return mCustomTextureVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::Effect::OnResize(unsigned int clientWidth, unsigned int clientHeight)
{

	//custom texture들의 사이즈를 변경한다.'

	for (int i = 0; i < mCustomTextureVector.size(); ++i)
	{

		Texture* texture = mCustomTextureVector[i];

		//사이즈 계산
		float widthRate = mWindowResizeRateCustomTextureVector[i].first;
		float heightRate = mWindowResizeRateCustomTextureVector[i].second;

		if (widthRate == 0.0f)	//하나라도 0이면 고정
			continue;

		unsigned int newWidth = clientWidth * widthRate;
		unsigned int newHeight = clientHeight * heightRate;

		TextureManager::ResizeTexture(mCustomTextureVector[i],newWidth,newHeight);
		//추가적으로 새로 설정할것이없음
		//texture의 내부리소스,view만 바뀐것이기에 //다른것들은 여전히 유지됨 
	}


}

void Quad::Effect::ResetRenderPassShaderResource()
{
	for (auto RenderPassTwoElement : mStaticMeshRenderPassVector)
	{

		const std::vector<ShaderResource*>& shaderResourceVector = RenderPassTwoElement->GetShaderResourceVector();

		for (int shaderResourceIndex = 0; shaderResourceIndex < shaderResourceVector.size(); ++shaderResourceIndex)
		{
			if (shaderResourceVector[shaderResourceIndex]->GetShaderResourceType() == EShaderResourceType::eConstantBuffer)
			{
				

				ShaderResourceConstantBuffer* shaderResourceCosntantBuffer = (ShaderResourceConstantBuffer*)
					shaderResourceVector[shaderResourceIndex];
				shaderResourceCosntantBuffer->Reset();


			}
		}

	}






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




