#pragma once



#include"Utility/Singleton.h"
#include"header.h"
namespace Quad
{

	class GraphicPipelineStateGeneratorHelper:public Singleton<GraphicPipelineStateGeneratorHelper>
	{
	public:
		GraphicPipelineStateGeneratorHelper();
		~GraphicPipelineStateGeneratorHelper();


		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device);


		void StartGenerateGraphicPipelineState(ID3D12RootSignature * rootSignature);

		void SetShaderCode(const void* vsByteCode, SIZE_T vsByteCodeLength,
							const void* psByteCode, SIZE_T psByteCodeLength,
							const void* gsByteCode = nullptr, SIZE_T gsByteCodeLength = 0,
							const void* dsByteCode =nullptr , SIZE_T dsByteCodeLength = 0,
							const void* hsByteCode=nullptr, SIZE_T hsByteCodeLength = 0);




		void SetVSCode(const void* vsByteCode, SIZE_T vsByteCodeLength);
		void SetPSCode(const void* psByteCode, SIZE_T psByteCodeLength);
		void SetGSCode(const void* gsByteCode, SIZE_T gsByteCodeLength);
		void SetDSCode(const void* dsByteCode, SIZE_T dsByteCodeLength);
		void SetHSCode(const void* hsByteCode, SIZE_T hsByteCodeLength);








		void SetInputLayout(const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs,
								UINT NumElements);

		void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType);


		void SetRenderTargetNum(int num);

		void SetRenderTargetViewFormat(int index, DXGI_FORMAT format);
		void SetDepthStencilViewFormat(DXGI_FORMAT format);


		void SetDepthStencilViewDesc(BOOL  depthEnable,
			D3D12_DEPTH_WRITE_MASK     depthWriteMask,
			D3D12_COMPARISON_FUNC      depthFunc,
			BOOL                       stencilEnable,
			UINT8                      stencilReadMask,
			UINT8                      stencilWriteMask,
			D3D12_DEPTH_STENCILOP_DESC frontFace,
			D3D12_DEPTH_STENCILOP_DESC backFace);

		void SetDepthStencilViewDesc(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc);



		void SetBlendDesc(bool alphaToCoverageEnable,
			bool  IndependentBlendEnable, const std::vector<D3D12_RENDER_TARGET_BLEND_DESC>& RenderTargetVector);
			

		//void SetRasterizerCullModeDesc(D3D12_FILL_MODE  fillMode,
		//						D3D12_CULL_MODE   cullMode = D3D12_CULL_MODE_BACK,
		//						BOOL   frontCounterClockwise = 0, 
		//						INT  depthBias= 0,
		//						FLOAT  depthBiasClamp = 0,
		//						FLOAT slopeScaledDepthBias = 0,
		//						BOOL  depthClipEnable = 0,
		//						BOOL  multisampleEnable = 0,
		//						BOOL  antialiasedLineEnable= 0,
		//						UINT  forcedSampleCount = 0,
		//	D3D12_CONSERVATIVE_RASTERIZATION_MODE conservativeRaster = 0);

		void SetRasterizerCullModeDesc(D3D12_CULL_MODE cullMode);
		void SetRasterizerFillModeDesc(D3D12_FILL_MODE  fillMode);



		Microsoft::WRL::ComPtr < ID3D12PipelineState > EndGenerateGraphicPipelineState();


	private:
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		D3D12_GRAPHICS_PIPELINE_STATE_DESC mGraphicsPipelineStateDesc;




	};

}
