#include "RootSignatureGeneratorHelper.h"
#include"Utility/DoException.h"
Quad::RootSignatureGeneratorHelper::RootSignatureGeneratorHelper()
{
}

Quad::RootSignatureGeneratorHelper::~RootSignatureGeneratorHelper()
{
}

void Quad::RootSignatureGeneratorHelper::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device)
{
	mDevice = device;
}

void Quad::RootSignatureGeneratorHelper::StartGenerateRootSignature(int rootParamterNum, D3D12_ROOT_SIGNATURE_FLAGS flag,
	 const std::vector<D3D12_STATIC_SAMPLER_DESC>* staticSamplerDescVector)
{
	mRootParameterVector.clear();
	mStaticSamplerVector.clear();
	mDescriptorRangeVector.clear();
	mRootSignatureDesc.NumStaticSamplers = 0;
	mRootSignatureDesc.pStaticSamplers = nullptr;

	mRootParameterVector.resize(rootParamterNum);
	mRootSignatureDesc.NumParameters = rootParamterNum;
	mRootSignatureDesc.Flags = flag;

	if (staticSamplerDescVector != nullptr)
	{
		mStaticSamplerVector = *staticSamplerDescVector;
		mRootSignatureDesc.NumStaticSamplers = mStaticSamplerVector.size();
		mRootSignatureDesc.pStaticSamplers = mStaticSamplerVector.data();
	}

	mDescriptorRangeVector.resize(rootParamterNum);

}

void Quad::RootSignatureGeneratorHelper::SetRootParameter(int parameterIndex, D3D12_ROOT_PARAMETER_TYPE parameterType, D3D12_SHADER_VISIBILITY shaderVisibility)
{
	mRootParameterVector[parameterIndex].ParameterType = parameterType;
	mRootParameterVector[parameterIndex].ShaderVisibility = shaderVisibility;

	

}

void Quad::RootSignatureGeneratorHelper::SetRootParameterTable(int parameterIndex, UINT descriptorRangeNum)
{
	mRootParameterVector[parameterIndex].DescriptorTable.NumDescriptorRanges = descriptorRangeNum;	
	//std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRangeVector(descriptorRangeNum);
	mDescriptorRangeVector[parameterIndex].resize(descriptorRangeNum);


}

void Quad::RootSignatureGeneratorHelper::SetRootParameterConstants(int parameterIndex, UINT shaderRegister, UINT registerSpace, UINT num32BitValues)
{


	mRootParameterVector[parameterIndex].Constants.ShaderRegister = shaderRegister;
	mRootParameterVector[parameterIndex].Constants.RegisterSpace = registerSpace;
	mRootParameterVector[parameterIndex].Constants.Num32BitValues = num32BitValues;

}

void Quad::RootSignatureGeneratorHelper::SetRootParameterDescriptor(int parameterIndex, UINT shaderRegister, UINT registerSpace)
{
	mRootParameterVector[parameterIndex].Descriptor.RegisterSpace = registerSpace;
	mRootParameterVector[parameterIndex].Descriptor.ShaderRegister = shaderRegister;

}


void Quad::RootSignatureGeneratorHelper::SetRootParameterTableRange(int parameterIndex, int rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, UINT descriptorNum, UINT baseShaderRegister, UINT registerSpace, UINT   offsetInDescriptorsFromTableStart)
{

	mDescriptorRangeVector[parameterIndex][rangeIndex].RangeType = rangeType;
	mDescriptorRangeVector[parameterIndex][rangeIndex].NumDescriptors = descriptorNum;
	mDescriptorRangeVector[parameterIndex][rangeIndex].BaseShaderRegister = baseShaderRegister;
	mDescriptorRangeVector[parameterIndex][rangeIndex].RegisterSpace = registerSpace;
	mDescriptorRangeVector[parameterIndex][rangeIndex].OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
}

Microsoft::WRL::ComPtr< ID3D12RootSignature> Quad::RootSignatureGeneratorHelper::EndGenerateRootSignature()
{

	for (size_t i = 0; i < mRootParameterVector.size(); ++i)
	{
		if (mRootParameterVector[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			mRootParameterVector[i].DescriptorTable.pDescriptorRanges = mDescriptorRangeVector[i].data();
		}


	}
	mRootSignatureDesc.pParameters = mRootParameterVector.data();






	///mRootSignatureDesc

	Microsoft::WRL::ComPtr< ID3D12RootSignature> rootSignature;

	Microsoft::WRL::ComPtr<ID3DBlob> serializeRootSingnatureBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> serializeRootSingnatureErrorBlob;

	HRESULT hresult = D3D12SerializeRootSignature(&mRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		serializeRootSingnatureBlob.GetAddressOf(), serializeRootSingnatureErrorBlob.GetAddressOf());

	if (hresult != S_OK)
	{
		OutputDebugStringA(reinterpret_cast<char*>(serializeRootSingnatureErrorBlob->GetBufferPointer()));
		assert(0);
	}

	ThrowIfFailed(mDevice->CreateRootSignature(0, serializeRootSingnatureBlob->GetBufferPointer(),
		serializeRootSingnatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf())));

	return rootSignature;


}




