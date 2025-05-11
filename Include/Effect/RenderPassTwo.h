#pragma once

#include"Predeclare.h"
#include"header.h"
///#include"ShaderResourceConstantBuffer.h"


#include"Effect/EffectDllExport.h"

namespace Quad
{




	class  RenderPassTwo
	{

	public:
		RenderPassTwo() = default;
		~RenderPassTwo();
		RenderPassTwo(const RenderPassTwo& renderPass);



		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState() const;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const;
		void SetPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState);
		void SetRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);


		void AddShaderResourece(ShaderResource* shaderResource);
		ShaderResource* GetShaderResource(const std::wstring& name)const ;
		const std::vector<ShaderResource*>& GetShaderResourceVector()const;


		void SetPrimitiveToplogyType(D3D_PRIMITIVE_TOPOLOGY type);
		D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopologyType() const;
		
		ERenderPassID GetRenderPassID()const;
		void SetRenderPassID(ERenderPassID id);


		void SetStencilWriteFlag(bool flag);
		bool GetStencilWriteFlag() const;


	private:




	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
		D3D_PRIMITIVE_TOPOLOGY mPrimitiveTopologyType;
		std::vector<ShaderResource*> mShaderResourceVector;


		ERenderPassID mRenderPassID;


		//스텐실을사용하고 write(스텐실값을 변경하도록 설정되어있는지에대한 flag)
		//이것이 true일경우 본견적인 렌더링전에 스텐실버퍼에만 렌더링하는작업을 수행하게된다. 이 renderpass를 가지는 effect는
		bool mStencilWriteFlag = false;


	};

}
