#pragma once

#include"Predeclare.h"
#include<fstream>
#include<string>
#include<vector>
#include"RawResourceType.h"
#include"header.h"
#include"Shader/ShaderResource.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB EffectLoader
	{	
	public:
		EffectLoader() = default;
		~EffectLoader() = default;

		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device);

		bool Load(const std::string& filePath);




	private:


		bool LoadRenderPassTwo(std::ifstream& fin, RawRenderPassDataTwo& oRawRenderPassDataTwo);
		bool LoadShaderDataTwo(std::ifstream& fin, RawShaderDataTwo& oRawShaderDataTwo);


		//effect를 구현해서 테이블에 등록한다.
		void ImplementEffect(RawEffectDataTwo & effectData);
		RenderPassTwo *CreateRenderPass(RawRenderPassDataTwo& rawRenderPassTwoData, bool isSkinningShader);

		Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::string& fileName, const D3D_SHADER_MACRO* pMacro,
			ID3DInclude* pInclude, const std::string& entryPoint, const std::string& version, UINT flags1,
			UINT flag2);

		void ShaderResourceReflection(RenderPassTwo* renderPass, Shader* shader, EShaderResourceVisibility shaderResourceVisibility);
		void SetObjectOrPassFlag(ShaderResourceConstantBuffer* shaderResourceConstantBuffer);
		UINT GetDefaultVariableSize(const D3D12_SHADER_TYPE_DESC& variableTypeDesc);
		UINT GetDefaultVariableElementSize(D3D_SHADER_VARIABLE_TYPE type); //defaultVariableSize의 헬퍼

		bool CreateRootSignature(RenderPassTwo* renderPass);
		bool CreateGraphicsPipeline(const RawRenderPassDataTwo& rawRenderPassTwoData,
			RenderPassTwo* renderPass, const std::vector<Shader*>& shaderVector,
			Shader* vertexShader);
		void SetGraphicsPipelineCfg(RenderPassTwo* renderPass,
			D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipelineStateDesc, const std::vector<std::pair<std::string, std::string>>& graphicsPipelineCfgVector);



		D3D12_SHADER_VISIBILITY ConvertD3DShaderVisibility(EShaderResourceVisibility visibility);
		D3D12_DESCRIPTOR_RANGE_TYPE ConvertD3DShaderRangeType(ShaderResource* shaderResource);


		UINT GetInputElementOffset(const std::string& sementicName);
		DXGI_FORMAT GetInputElementFormat(const std::string& sementicName,
			D3D_REGISTER_COMPONENT_TYPE componentType, BYTE mask);
		//모든것을 포함하는 버택스의 정의가있어야하고 
		//그것을 바탕으로 offset값을 계산한

		D3D12_STENCIL_OP ConvertStencilOP(const std::string& str, RenderPassTwo* renderPass) const;
		D3D12_COMPARISON_FUNC ConvertComparisionFunEnum(const std::string& str) const;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertPrimitiveTopologyType(const std::string& str) const;
		D3D_PRIMITIVE_TOPOLOGY ConvertCommandListPrimitiveTopologyType(const std::string& str) const;



	private:
		D3D12_BLEND ConvertBlendFactorEnum(const std::string& str);
		D3D12_BLEND_OP ConvertBlendOpEnum(const std::string& str);
		D3D12_STENCIL_OP ConvertStencilOpEnum(const std::string& str);
		D3D12_COMPARISON_FUNC ConvertComparisonFuncEnum(const std::string& str);

		D3D12_SRV_DIMENSION ConvertSRVDimensionEnum(D3D12_RESOURCE_DIMENSION dimension);

		D3D12_CULL_MODE ConvertCullMode(const std::string& str);


		ERenderPassID ConvertRenderPassID(const std::string& str) const;
		D3D12_BLEND_OP ConvertD3DBlendOpEnum(const std::string& str);
		void InitBlendEnumUnMap();
		void InitStencilEnumUnMap();
		void InitComparisonFuncEnumUnMap();



		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;


		std::unordered_map<std::string, D3D12_BLEND> mBlendFactorEnumUnMap;
		std::unordered_map<std::string, D3D12_BLEND_OP > mBlendOpEnumUnMap;
		std::unordered_map<std::string, D3D12_STENCIL_OP> mStencilOpEnumUnMap;
		std::unordered_map<std::string, D3D12_COMPARISON_FUNC> mComparisonFuncEnumUnMap;
	};
}

