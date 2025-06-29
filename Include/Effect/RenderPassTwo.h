#pragma once

#include"Predeclare.h"
#include"header.h"
///#include"ShaderResourceConstantBuffer.h"


#include"Effect/EffectDllExport.h"

#include"RenderPassCommand.h"
#include<variant>

namespace Quad
{


	enum class ERenderPassDrawType
	{


		eObjectDraw=0,
		eEntirePreDraw,
		eEntirePostDraw
	};



	class  RenderPassTwo
	{

	public:
		RenderPassTwo();
		~RenderPassTwo();
		RenderPassTwo(const RenderPassTwo& renderPass);


		void SetDestEffect(Effect* effect);
		Effect* GetDestEffect() const;


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

		void SetCustomRenderTarget(RenderTargetTexture* renderTargetTexture);
		RenderTargetTexture* GetCustomRenderTarget() const;

		//실제 texture의 uniqueID가 아닌 effect내에서의 id
		void SetCustomRenderTargetLocalID(int id);
		int GetCustomRenderTargetLocalID() const;

		void AddPreRenderPassCommand(RenderPassCommand* renderPassCommmand);
		void AddPostRenderPassCommand(RenderPassCommand* renderPassCommand);

		void SetPreRenderPassCommandVector(const std::vector<RenderPassCommand*>& renderPassCommandVector);
		void SetPreRenderPassCommandVector(std::vector<RenderPassCommand*>&& renderPassCommandVector);

		void SetPostRenderPassCommandVector(const std::vector<RenderPassCommand*>& renderPassCommandVector);
		void SetPostRenderPassCommandVector(std::vector<RenderPassCommand*>&& renderPassCommandVector);


		const std::vector<RenderPassCommand*>& GetPreRenderPassCommandVector() const;
		const std::vector<RenderPassCommand*>& GetPostRenderPassCommandVector() const;


		void SetRenderPassDrawType(ERenderPassDrawType renderPassDrawType);
		ERenderPassDrawType GetRenderPassDrawType() const;


	private:

		Effect* mDestEffect;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
		D3D_PRIMITIVE_TOPOLOGY mPrimitiveTopologyType;
		std::vector<ShaderResource*> mShaderResourceVector;


		ERenderPassID mRenderPassID;


		//스텐실을사용하고 write(스텐실값을 변경하도록 설정되어있는지에대한 flag)
		//이것이 true일경우 본견적인 렌더링전에 스텐실버퍼에만 렌더링하는작업을 수행하게된다. 이 renderpass를 가지는 effect는
		bool mStencilWriteFlag = false;


		bool mCustomRenderTargetUsage = false;
		//nullptr이라면 렌더시스템은 이 렌더패스에서 렌더타켓은 기본 렌더타켓을 사용한다.
		RenderTargetTexture * mCustomRenderTargetTexture;
		int mCustomRenderTargetLocalID;	//디폴트 0 : 


		std::vector< RenderPassCommand*> mPreRenderPassCommandVector;
		std::vector< RenderPassCommand*> mPostRenderPassCommandVector;

		//std::vector< std::variant<RenderPassCommand>> mPreRenderPassCommandVector;
		ERenderPassDrawType mRenderPassDrawType;

	};

}
