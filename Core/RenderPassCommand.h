#pragma once

#include"Asset/Texture/Texture.h"

#include"Shader/ShaderResourceType.h"
#include"Allocator/DestructingPoolAllocator.h"
namespace Quad
{ 

	enum class ERenderPassCommandType
	{
		eTextureCopy = 0,
		eTextureBind,
		eResourceStateTransition,
		eRenderTargetSetting
	};

	class RenderPassCommand
	{
	public:
		RenderPassCommand(ERenderPassCommandType renderPassCommandType);
		virtual ~RenderPassCommand() = default;


		ERenderPassCommandType GetRenderPassCommandType() const;

	private:
		ERenderPassCommandType mCommandType;
	
	};


	class RenderPassTextureCopyCommnad :public RenderPassCommand
	{
	public:
		RenderPassTextureCopyCommnad();
		virtual ~RenderPassTextureCopyCommnad() = default;
		RenderPassTextureCopyCommnad(const RenderPassTextureCopyCommnad& renderPassCommand);	//id만 복사됨


		//-1 이면 기본 렌더타겟
		void SetSourceTextureLocalID(int id);
		//-1이면 기본 렌더타켓
		void SetDestTextureLocalID(int id);

		int GetSourceTextureLocalID() const;
		int GetDestTextureLocalID() const;

		void SetSourceTexture(Texture* sourceTexture);
		void SetDestTexture(Texture* destTexture);

		Texture* GetSourceTexture()const;
		Texture* GetDestTexture()const;

	private:
		Texture* mSourceTexture;		//nullptr이면 기본 렌더타겟
		Texture* mDestTexture;			//nullptr이면 기본 렌더타겟

		int mSourceTextureLocalID;		//-1이면 기본 렌더타겟 
		int mDestTextureLocalID;		//-1이면 기본 렌더타켓

	};

	class RenderPassTextureBind: public RenderPassCommand
	{
	public:
		RenderPassTextureBind();
		virtual ~RenderPassTextureBind() = default;


		//texture 는 nullptr이될수없음, 기본렌더타겟은 허용안함.
		void SetTextureSrv(EShaderResourceDescriptorType shaderResourceType, Texture* texture,int rootParamter);
		void SetTextureSrv(EShaderResourceDescriptorType shaderResourceType, int textureLocalID,int rootParamter);
		



		void SetTexture(Texture* texture);
		Texture * GetTexture() const;
		int GetRootParameter() const;
		int GetTextureLocalID() const;

	private:
		EShaderResourceDescriptorType mShaderResourceDescriptorType;
		int mRootParameter;
		int mTextureLocalID = -1;
		Texture* mTexture;




	};

	class RenderPassResourceStateTransitionCommand:public RenderPassCommand
	{
	public:
		RenderPassResourceStateTransitionCommand();
		virtual ~RenderPassResourceStateTransitionCommand() = default;

		void SetStateTransition(D3D12_RESOURCE_STATES sourceState, D3D12_RESOURCE_STATES destState);

		D3D12_RESOURCE_STATES GetStateBefore() const;
		D3D12_RESOURCE_STATES GetStateAfter() const;

		//이거는 엔진에서사용하기위한것
		//-1은 기본렌더타겟
		void SetTextureLocalID(int localID);
		int GetTextureLocalID()const;

		//유저는 이것을 사용
		void SetTextureUniqueID(unsigned long long id);

		//nullptr이면 기본렌더타겟
		void SetTexture(Texture* texture);
		Texture* GetTexture() const;

		
	private:
		D3D12_RESOURCE_STATES mSourceState;
		D3D12_RESOURCE_STATES mDestState;

		int mTextureLocalID;
		int mTextureUniqueID;

		Texture* mTexture;

	};


	class RenderPassSetRenderTargetCommand :public RenderPassCommand
	{
	public:
		RenderPassSetRenderTargetCommand();
		virtual ~RenderPassSetRenderTargetCommand() = default;



		void SetRenderTargetTextureLocalID(int textureLocalID);
		int GetRenderTargetTextureLocalID() const;


		void SetDepthStencilBufferLocalID(int dsBufferLocalID);
		int GetDepthStencilBufferLocalID() const;



		void SetRenderTargetTexture(RenderTargetTexture* texture);
		RenderTargetTexture* GetRenderTargetTexture() const;
	

		void SetDepthStencilBuffer(Texture* dsBuffer);
		Texture * GetDepthStencilBuffer() const;

		void SetRenderTargetUsage(bool flag);
		void SetDepthStencilBufferUsage(bool flag);

		bool GetRenderTargetUsage() const;
		bool GetDepthStencilBufferUsage() const;


	private:
		int mTextureLocalID;
		int mDepthStencilLocalID;
		RenderTargetTexture* mTexture;

		//지금은 임시로 texture이고 후에 dsv를 가지는 texture클래스를 만들어야한다.
		Texture* mDepthStencilBuffer;

		bool mRenderTargetUsage;
		bool mDepthStencilBufferUsage;

	};




	class RenderPassCommandFactory
	{
	public:
		static RenderPassCommandFactory* GetInstance();

		~RenderPassCommandFactory();

		RenderPassCommand* CreateRenderPassCommand(ERenderPassCommandType renderPassCommandType);

		void ReleaseRenderPassCommand(RenderPassCommand* renderPassCommand);

	private:
		RenderPassCommandFactory();


		DestructingPoolAllocator< RenderPassTextureCopyCommnad> mTextureCopyCommandAllocator;
		DestructingPoolAllocator< RenderPassTextureBind> mTextureBindCommandAllocator;
		DestructingPoolAllocator< RenderPassResourceStateTransitionCommand> mResourceStateTransitionCommandAllocator;
		DestructingPoolAllocator< RenderPassSetRenderTargetCommand> mRenderPassSetRenderTargetCommandAllocator;





	};






}
