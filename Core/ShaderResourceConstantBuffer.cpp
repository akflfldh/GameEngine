#include "Shader/ShaderResourceConstantBuffer.h"
///#include"Object/Object.h"
//#include"PassData.h"
//#include"Camera.h"
//#include"Light.h"
//#include"RenderSystem.h"
//#include"Material.h"
//#include"Entity.h"
//#include"LineBase.h"
//#include"LineBaseMaterial.h"
#include"Buffer/ConstantBuffer.h"

Quad::ShaderResourceConstantBuffer::ShaderResourceConstantBuffer(const std::wstring & name,
	char registerIndex, char registerSpace)
	:ShaderResource(EShaderResourceType::eConstantBuffer,name,registerIndex,registerSpace), mDataTempBuffer(nullptr)
{
}

Quad::ShaderResourceConstantBuffer::ShaderResourceConstantBuffer(const ShaderResourceConstantBuffer& shaderResourceConstantBuffer) :ShaderResource(shaderResourceConstantBuffer)
{

	mVariableVector = shaderResourceConstantBuffer.mVariableVector;
	mConstantBuffer = shaderResourceConstantBuffer.mConstantBuffer;
	mConstantBufferOffset = 0;
	mCurrenntUploadCount = 0;
	mDataSize = shaderResourceConstantBuffer.mDataSize;
	mDataTempBuffer = new char[mDataSize];
	mStructTypeDescUnMapTable = shaderResourceConstantBuffer.mStructTypeDescUnMapTable;

}



void Quad::ShaderResourceConstantBuffer::Initialize(const std::vector<ShaderResourceVariable>& variableVector, UINT dataSize)
{
	mVariableVector = variableVector;
	mDataSize = dataSize;
	mDataTempBuffer = new char[mDataSize];
}

void Quad::ShaderResourceConstantBuffer::Initialize(std::vector<ShaderResourceVariable>&& variableVector, UINT dataSize)
{
	mVariableVector = std::move(variableVector);
	mDataSize = dataSize;
	mDataTempBuffer = new char[mDataSize];
}



void Quad::ShaderResourceConstantBuffer::SetConstantBuffer(ConstantBuffer* constantBuffer)
{
	if (constantBuffer == nullptr)
	{
		MessageBox(nullptr, L"상수버퍼가 nullptr", L"errro", 0);
		assert(0);
	}

	mConstantBuffer = constantBuffer;
}

void Quad::ShaderResourceConstantBuffer::UploadData(RenderItem* renderItem, const PassData & passData)
{
//	//업로드되는 순서대로 렌더링하면된다.
//	
//	//upload
//
//	//변수 이름보고 
//	//offset보고 
//	//적절하게 버퍼채워서 
//	//상수버퍼에 업로드하는거지
//	//
//	SceneElement* object = nullptr;
//	if(renderItem!=nullptr)
//		object = renderItem->mSceneElement;
//
//
//
//	for (int variableIndex = 0; variableIndex < mVariableVector.size(); ++variableIndex)
//	{
//		
//		const std::wstring & name = mVariableVector[variableIndex].mName;
//		UINT mDataOffsetInBuffer = mVariableVector[variableIndex].mOffsetInShaderResource;
//		UINT mDataSize = mVariableVector[variableIndex].mSize;
//		D3D_SHADER_VARIABLE_CLASS variableClass = mVariableVector[variableIndex].mClass;
//		int elementNum = mVariableVector[variableIndex].mElementNum;
//
//		void* src = nullptr;
//		if (name == L"gWorld")
//		{
//			DirectX::XMFLOAT4X4 gWorldMatrix = object->GetTransform().GetWorldMatrix();
//
//			DirectX::XMFLOAT4X4 gWorldMatrixTransPose;
//			DirectX::XMStoreFloat4x4(&gWorldMatrixTransPose,DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&gWorldMatrix)));
//			src = &gWorldMatrixTransPose;
//			memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gWorldMatrixTransPose, mDataSize);	
//		}
//		else if (name == L"gWorldInvTrans")
//		{
//
//			DirectX::XMFLOAT4X4 gWorldMatrix = object->GetTransform().GetWorldMatrix();
//		
//			DirectX::XMFLOAT4X4 gWorldMatrixInvTransPose;
//			DirectX::XMStoreFloat4x4(&gWorldMatrixInvTransPose, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&gWorldMatrix)));
//			src = &gWorldMatrixInvTransPose;
//			memcpy(&mDataTempBuffer[mDataOffsetInBuffer], src, mDataSize);
//		}
//		else if (name == L"gEye")
//		{
//			DirectX::XMFLOAT3 gEye = passData.mCamera->GetTransform().GetPositionWorld();
//			src = &gEye;
//			memcpy(&mDataTempBuffer[mDataOffsetInBuffer], src, mDataSize);
//		}
//		else if (name == L"gViewProj")
//		{
//			DirectX::XMFLOAT4X4 gViewProj = passData.mCamera->GetViewProjMatrix();
//			DirectX::XMFLOAT4X4 gViewProjTranspose;
//			DirectX::XMStoreFloat4x4(&gViewProjTranspose, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&gViewProj)));
//			src = &gViewProjTranspose;
//
//			memcpy(&mDataTempBuffer[mDataOffsetInBuffer], src, mDataSize);
//		}
//		else if (name == L"gDirectionalLight")
//		{
//			//빛은 Light라는 구조체 타입이여하고 무조건
//
//			const std::wstring& structTypeName = mVariableVector[variableIndex].mTypeName;
//			const ShaderResourceStructTypeDesc * pShaderResourceStructTypeDesc =	GetShaderResourceStructDesc(structTypeName);
//
//
//			UploadLightStructData(mDataTempBuffer, pShaderResourceStructTypeDesc, elementNum,
//				mDataOffsetInBuffer,passData,ELightType::eDirect);
//
//
//		}
//		else if (name == L"gSpotLight")
//		{
//			const std::wstring& structTypeName = mVariableVector[variableIndex].mTypeName;
//			const ShaderResourceStructTypeDesc* pShaderResourceStructTypeDesc = GetShaderResourceStructDesc(structTypeName);
//
//
//			UploadLightStructData(mDataTempBuffer, pShaderResourceStructTypeDesc, elementNum,
//				mDataOffsetInBuffer, passData, ELightType::eSpot);
//
//		}
//		else if (name == L"gPointLight")
//		{
//			const std::wstring& structTypeName = mVariableVector[variableIndex].mTypeName;
//			const ShaderResourceStructTypeDesc* pShaderResourceStructTypeDesc = GetShaderResourceStructDesc(structTypeName);
//
//
//			UploadLightStructData(mDataTempBuffer, pShaderResourceStructTypeDesc, elementNum,
//				mDataOffsetInBuffer, passData, ELightType::ePoint);
//
//		}
//		else if (name == L"gFresnelR0")
//		{
//
//			const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
//
//			DirectX::XMFLOAT3 gFresnelR0 = modelSubMesh->mModelMaterial.GetFresnelR0();
//
//			memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gFresnelR0, mDataSize);
//
//		}
//		else if (name == L"gShiness")
//		{
//			const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
//			float gShiness = modelSubMesh->mModelMaterial.GetShiness();
//
//			memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gShiness, mDataSize);
//		}
//		else if (name == L"gSpecular")
//		{
//			const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
//			DirectX::XMFLOAT3 gSpecular = modelSubMesh->mModelMaterial.GetSpecular();
//
//			memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gSpecular, mDataSize);
//		}
//		else if (name == L"gAmbient")
//		{
//			const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
//			DirectX::XMFLOAT3 gAmbient = modelSubMesh->mModelMaterial.GetAmbient();
//
//			memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gAmbient, mDataSize);
//		}
//		else if (name == L"gColor")
//		{
//
//
//			DirectX::XMFLOAT3 gColor;
//			
//			if (renderItem->mSceneElement->GetSceneElementType() == ESceneElementType::eCollider)
//			{
//				const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
//				gColor = modelSubMesh->mModelMaterial.GetColor();
//			}
//			else if(renderItem->mSceneElement->GetSceneElementType() == ESceneElementType::eObject)
//			{
//				Object* object = static_cast<Object*>(renderItem->mSceneElement);
//
//				switch (object->GetObjectType())
//				{
//				case EObjectType::eEntity:
//				{
//					const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
//					gColor = modelSubMesh->mModelMaterial.GetColor();
//					if (gColor.x == 0)
//					{
//						int a = 2;
//					}
//				}
//				break;
//				case EObjectType::eLine:
//				{
//					const LineModelSubMesh* modelSubMesh = static_cast<const LineModelSubMesh*>(renderItem->mSubMesh);
//					LineBase* lineBase = (LineBase*)renderItem->mSceneElement;
//					gColor = modelSubMesh->mModelMaterial.GetColor();
//				}
//				break;
//				}
//			}
//			memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gColor, mDataSize);
//
//
//
//		}
//		else if (name == L"gColorIntensity")
//		{
//			DirectX::XMFLOAT3 gColorIntensity;
//
//			if(renderItem->mSceneElement->GetSceneElementType() == ESceneElementType::eObject)
//			{ 
//				Object* object = static_cast<Object*>(renderItem->mSceneElement);
//				switch (object->GetObjectType())
//				{
//				case EObjectType::eEntity:
//				{
//					const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
//					gColorIntensity = modelSubMesh->mModelMaterial.GetColorIntensity();
//				}
//				break;
//			}
//		}
//
//		memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gColorIntensity, mDataSize);
//
//
//
//		}
//		else if (name == L"gTextureTrans0")
//		{
//			const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
//			DirectX::XMFLOAT4X4  textureTransformMatrix = modelSubMesh->mModelMaterial.GetTextureTransformMatrix(0);
//			DirectX::XMStoreFloat4x4(&textureTransformMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&textureTransformMatrix)));
//
//			memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &textureTransformMatrix, mDataSize);
//
//
//		}
//		else if (name == L"gDiffuseMapGammaFlag")
//		{
//			const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
//			ETextureFormat textureFormat = modelSubMesh->mModelMaterial.GetDiffuseMap()->GetTextureFormat();
//			float flag;
//
//			if (textureFormat == ETextureFormat::eRgb)
//			{
//				flag = 0.0f;
//			}
//			else if(textureFormat==ETextureFormat::eSRgb)
//			{
//				flag = 1.0f;
//			}
//
//			memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &flag, mDataSize);
//		}
//		else if (name == L"gBoneAnimationMatrix")
//		{
//			//엔티티만일단 가능하다고 보자
//			//그냥 정적메시도 단일 본이라고 생각할수도있겠지만 다음에 생각하자
//			
//
//			Entity* entity = dynamic_cast <Entity*> (object);
//			if (entity == nullptr)
//			{
//				OutputDebugString(L"Entity가아닌 오브젝트에 에니메이션effect를 사용할려고한다.\n");
//				assert(0);
//			}
//
//			const AnimationComponent& animComponent = *entity->GetModel()->GetAnimationComponent();
//			const std::vector<DirectX::XMFLOAT4X4>& finalTransformVector = animComponent.GetFinalTransformMatrixVector();
//			//transpose가 된 상태(여기서 복사해서 전치하면 비용발생하니) 
//			if (finalTransformVector.size() > elementNum)
//			{
//				OutputDebugString(L"뼈대팔렛트행렬의수가 HLSL에 정의된 수보다 많다.\n");
//				assert(0);
//			}
//
//			memcpy(&mDataTempBuffer[mDataOffsetInBuffer], finalTransformVector.data(), sizeof(DirectX::XMFLOAT4X4)* finalTransformVector.size());
//
//		}
//		else if (name == L"gBoneAnimationInverseTransposeMatrix")
//		{
//			
//			Entity* entity = dynamic_cast <Entity*> (object);
//			if (entity == nullptr)
//			{
//				OutputDebugString(L"Entity가아닌 오브젝트에 에니메이션effect를 사용할려고한다.\n");
//				assert(0);
//			}
//
//			const AnimationComponent& animComponent = *entity->GetModel()->GetAnimationComponent();
//			const std::vector<DirectX::XMFLOAT4X4>& finalInverseTransposeTransformVector = animComponent.GetFinalInverseTransposeTransformMatrixVector();
//			//transpose가 된 상태(여기서 복사해서 전치하면 비용발생하니) 
//			if (finalInverseTransposeTransformVector.size() > elementNum)
//			{
//				OutputDebugString(L"뼈대팔렛트행렬의수가 HLSL에 정의된 수보다 많다.\n");
//				assert(0);
//			}
//
//			memcpy(&mDataTempBuffer[mDataOffsetInBuffer], finalInverseTransposeTransformVector.data(), sizeof(DirectX::XMFLOAT4X4) * finalInverseTransposeTransformVector.size());
//
//
//		}
//		//	float4x4 gBoneAnimationMatrix[120];
//	//	float4x4 gBoneAnimationInverseTransposeMatrix[120];
//
//
//	}
//
//	ViewIndex currentViewIndex = mConstantBuffer->UploadData(mDataTempBuffer, mDataSize);
//	mViewIndexVector.push_back(currentViewIndex);
//
//
//
//}
//
//void Quad::ShaderResourceConstantBuffer::UploadLightStructData(char * dataTempBuffer, const ShaderResourceStructTypeDesc* pShaderResourceStructTypeDesc,int elementNum, int structOffset, const PassData& passData , ELightType lightType)
//{
//	const std::vector<LightItem*>* pLightItemVector=nullptr;
//	switch (lightType)
//	{
//	case ELightType::eDirect:
//		pLightItemVector = &passData.mDirectionalLightItemVector;
//		break;
//	case ELightType::ePoint:
//		pLightItemVector = &passData.mPointSpotLightItemVector;
//		break;
//	case ELightType::eSpot:
//		pLightItemVector = &passData.mSpotLightItemVector;
//		break;
//	}
//	
//
//	
//	const std::vector<ShaderResourceVariable> structVariableVector = pShaderResourceStructTypeDesc->mVariableVector;
//	unsigned int structSize = pShaderResourceStructTypeDesc->mSize;
//	unsigned int structAlignedSize = pShaderResourceStructTypeDesc->mAlignedSize;
//
//
//	if (elementNum == 0)
//	{//배열이 아닌 그냥 단일변수 /원소가 하나뿐인 배열로 취급하자.
//		elementNum = 1;
//	}
//
//
//	for (int elementIndex = 0; elementIndex < elementNum; ++elementIndex)
//	{
//		if (elementIndex >= (*pLightItemVector).size())
//			return;
//
//		for (int structVariableIndex = 0; structVariableIndex < structVariableVector.size(); ++structVariableIndex)
//		{
//			const std::wstring& variableName = structVariableVector[structVariableIndex].mName;
//			int offset =  structVariableVector[structVariableIndex].mOffsetInShaderResource;
//			if (variableName == L"gLight")
//			{
//				DirectX::XMFLOAT3 light = (*pLightItemVector)[elementIndex]->mLight->GetLight();
//				memcpy(&dataTempBuffer[structOffset + offset], &light, structVariableVector[structVariableIndex].mSize);
//			}
//			else if (variableName == L"gDirection")
//			{
//
//				DirectX::XMFLOAT3 direction = (*pLightItemVector)[elementIndex]->mLight->GetLightDirect();
//				
//				memcpy(&dataTempBuffer[structOffset + offset], &direction, structVariableVector[structVariableIndex].mSize);
//			}
//			else if (variableName == L"gPosW")
//			{
//
//				DirectX::XMFLOAT3 positionW = (*pLightItemVector)[elementIndex]->mLight->GetPosition();
//				memcpy(&dataTempBuffer[structOffset + offset], &positionW, structVariableVector[structVariableIndex].mSize);
//			}
//
//		}
//		structOffset += structAlignedSize;
//	}



}

void Quad::ShaderResourceConstantBuffer::Reset()
{
	mViewIndexVector.clear();
	mCurrenntUploadCount = 0;

//	mConstantBuffer->ResetUpLoadCount();

}





ViewIndex Quad::ShaderResourceConstantBuffer::GetViewIndex(UINT uploadIndex) const
{
	return mViewIndexVector[uploadIndex];
}


void Quad::ShaderResourceConstantBuffer::AddShaderResourceStructDesc(ShaderResourceStructTypeDesc* structTypeDesc)
{
	mStructTypeDescUnMapTable.try_emplace(structTypeDesc->mName, structTypeDesc);
}


const Quad::ShaderResourceStructTypeDesc* Quad::ShaderResourceConstantBuffer::GetShaderResourceStructDesc(const std::wstring& name) const
{
	std::unordered_map<std::wstring, std::shared_ptr<ShaderResourceStructTypeDesc>>::const_iterator  it = mStructTypeDescUnMapTable.find(name);

	return it != mStructTypeDescUnMapTable.cend() ? it->second.get() : nullptr;

}

const std::vector<Quad::ShaderResourceVariable>& Quad::ShaderResourceConstantBuffer::GetShaderResourceVariableVector() const
{

	return mVariableVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::ShaderResourceConstantBuffer::UploadData(int variableIndex, void* data, size_t size)
{
	

	const std::wstring& name = mVariableVector[variableIndex].mName;
	UINT dataOffsetInBuffer = mVariableVector[variableIndex].mOffsetInShaderResource;

	UINT dataSize = mVariableVector[variableIndex].mSize;
	if (size != 0)
		dataSize = size;// size가 실제 정확한 source Data의 사이즈 0을 넘기면 동일하다고 판정(호출하는쪽을 편리하게하기위해)


	D3D_SHADER_VARIABLE_CLASS variableClass = mVariableVector[variableIndex].mClass;
	int elementNum = mVariableVector[variableIndex].mElementNum;


	memcpy(&mDataTempBuffer[dataOffsetInBuffer], data, dataSize);




}

void Quad::ShaderResourceConstantBuffer::UploadStructData(int variableIndex,int elementIndex , int structAlignSize, int offset, void* data, int dataSize)
{

	const std::wstring& name = mVariableVector[variableIndex].mName;
	UINT dataOffsetInBuffer = mVariableVector[variableIndex].mOffsetInShaderResource;
	//UINT dataSize = mVariableVector[variableIndex].mSize;
	D3D_SHADER_VARIABLE_CLASS variableClass = mVariableVector[variableIndex].mClass;
	int elementNum = mVariableVector[variableIndex].mElementNum;




	memcpy(&mDataTempBuffer[dataOffsetInBuffer+ elementIndex * structAlignSize + offset], data, dataSize);



}

void Quad::ShaderResourceConstantBuffer::EndUploadData()
{

	ViewIndex currentViewIndex = mConstantBuffer->UploadData(mDataTempBuffer, mDataSize);
	mViewIndexVector.push_back(currentViewIndex);
}
