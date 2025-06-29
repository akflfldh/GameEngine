#pragma once



#include"header.h"
#include"Utility/Singleton.h"
#include<vector>
namespace Quad
{

	class RootSignatureGeneratorHelper:public Singleton<RootSignatureGeneratorHelper>
	{
	public:
		RootSignatureGeneratorHelper();
		~RootSignatureGeneratorHelper();
		
		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device);

		//항상 새로운 root signature  만들기 시작할때 호출
		void StartGenerateRootSignature(int rootParamterNum, D3D12_ROOT_SIGNATURE_FLAGS flag,const  const std::vector<D3D12_STATIC_SAMPLER_DESC>* staticSamplerDescVector);
		
		


		void SetRootParameter(int parameterIndex, D3D12_ROOT_PARAMETER_TYPE  parameterType, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);



		void SetRootParameterTable(int paramterIndex,UINT descriptorRangeNum);
		void SetRootParameterConstants(int paramterIndex,UINT shaderRegister, UINT reigsterSpace, UINT num32BitValues);
		void SetRootParameterDescriptor(int paremterIndex,UINT shaderRegister, UINT registerSpace);


		void SetRootParameterTableRange(int parameterIndex, int rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, UINT descriptorNum ,UINT baseShaderRegister, UINT registerSpace,UINT   offsetInDescriptorsFromTableStart= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);



		//설정이 다되었다면 호출 
		Microsoft::WRL::ComPtr< ID3D12RootSignature> EndGenerateRootSignature();


	private:
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		std::vector<D3D12_ROOT_PARAMETER> mRootParameterVector;
		D3D12_ROOT_SIGNATURE_DESC mRootSignatureDesc;

		std::vector<std::vector<D3D12_DESCRIPTOR_RANGE>> mDescriptorRangeVector;

		std::vector<D3D12_STATIC_SAMPLER_DESC> mStaticSamplerVector;
	};


}