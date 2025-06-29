#include "RenderPassCommand.h"


Quad::RenderPassCommand::RenderPassCommand(ERenderPassCommandType renderPassCommandType)
	:mCommandType(renderPassCommandType)
{
}

Quad::ERenderPassCommandType Quad::RenderPassCommand::GetRenderPassCommandType() const
{
	return mCommandType;
}

Quad::RenderPassTextureCopyCommnad::RenderPassTextureCopyCommnad()
	:RenderPassCommand(ERenderPassCommandType::eTextureCopy),mSourceTexture(nullptr),mDestTexture(nullptr)
{

}

Quad::RenderPassTextureCopyCommnad::RenderPassTextureCopyCommnad(const RenderPassTextureCopyCommnad& renderPassCommand)
	:RenderPassCommand(renderPassCommand.GetRenderPassCommandType())
{
	mSourceTextureLocalID = renderPassCommand.mSourceTextureLocalID;
	mDestTextureLocalID = renderPassCommand.mDestTextureLocalID;

}

void Quad::RenderPassTextureCopyCommnad::SetSourceTextureLocalID(int id)
{
	mSourceTextureLocalID = id;

}

void Quad::RenderPassTextureCopyCommnad::SetDestTextureLocalID(int id)
{
	mDestTextureLocalID = id;
}

int Quad::RenderPassTextureCopyCommnad::GetSourceTextureLocalID() const
{
	return mSourceTextureLocalID;
}

int Quad::RenderPassTextureCopyCommnad::GetDestTextureLocalID() const
{
	return mDestTextureLocalID;
}

void Quad::RenderPassTextureCopyCommnad::SetSourceTexture(Texture* sourceTexture)
{
	mSourceTexture = sourceTexture;
}

void Quad::RenderPassTextureCopyCommnad::SetDestTexture(Texture* destTexture)
{
	mDestTexture = destTexture;
}

Quad::Texture* Quad::RenderPassTextureCopyCommnad::GetSourceTexture() const
{
	return mSourceTexture;
}

Quad::Texture* Quad::RenderPassTextureCopyCommnad::GetDestTexture() const
{
	return mDestTexture;
}

Quad::RenderPassTextureBind::RenderPassTextureBind()
	:RenderPassCommand(ERenderPassCommandType::eTextureBind)
{
}

void Quad::RenderPassTextureBind::SetTextureSrv(EShaderResourceDescriptorType shaderResourceType, Texture* texture, int rootParameter)
{

	mTexture = texture;
	mShaderResourceDescriptorType = shaderResourceType;
	mRootParameter = rootParameter;
	
}

void Quad::RenderPassTextureBind::SetTextureSrv(EShaderResourceDescriptorType shaderResourceType, int textureLocalID, int rootParamter)
{
	mTextureLocalID = textureLocalID;
	mShaderResourceDescriptorType = shaderResourceType;
	mRootParameter = rootParamter;

}

void Quad::RenderPassTextureBind::SetTexture(Texture* texture)
{
	mTexture = texture;
}

Quad::Texture* Quad::RenderPassTextureBind::GetTexture() const
{
	return mTexture;
}

int Quad::RenderPassTextureBind::GetRootParameter() const
{
	return mRootParameter;
}

int Quad::RenderPassTextureBind::GetTextureLocalID() const
{
	return mTextureLocalID;
}

Quad::RenderPassCommandFactory* Quad::RenderPassCommandFactory::GetInstance()
{
	static RenderPassCommandFactory renderPassCommandFactory;

	return &renderPassCommandFactory;
}

Quad::RenderPassCommandFactory::~RenderPassCommandFactory()
{
}

Quad::RenderPassCommand* Quad::RenderPassCommandFactory::CreateRenderPassCommand(ERenderPassCommandType renderPassCommandType)
{
	RenderPassCommand* renderPassCommand = nullptr;

	switch (renderPassCommandType)
	{
	case ERenderPassCommandType::eTextureCopy:
		renderPassCommand = mTextureCopyCommandAllocator.GetInstance();


		break;
	case ERenderPassCommandType::eTextureBind:
		renderPassCommand = mTextureBindCommandAllocator.GetInstance();
		break;
	case ERenderPassCommandType::eResourceStateTransition:
		renderPassCommand = mResourceStateTransitionCommandAllocator.GetInstance();
		break;
	case ERenderPassCommandType::eRenderTargetSetting:
		renderPassCommand = mRenderPassSetRenderTargetCommandAllocator.GetInstance();

	}

	return renderPassCommand;
}

void Quad::RenderPassCommandFactory::ReleaseRenderPassCommand(RenderPassCommand* renderPassCommand)
{
	if (renderPassCommand == nullptr)
		return;


	switch (renderPassCommand->GetRenderPassCommandType())
	{
	case ERenderPassCommandType::eTextureCopy:
		mTextureCopyCommandAllocator.ReleaseInstance(static_cast<RenderPassTextureCopyCommnad*>(renderPassCommand));


		break;
	case ERenderPassCommandType::eTextureBind:
		mTextureBindCommandAllocator.ReleaseInstance(static_cast<RenderPassTextureBind*>(renderPassCommand));
		break;
	case ERenderPassCommandType::eResourceStateTransition:
		mResourceStateTransitionCommandAllocator.ReleaseInstance(static_cast<RenderPassResourceStateTransitionCommand*>(renderPassCommand));
		break;
	case ERenderPassCommandType::eRenderTargetSetting:
		mRenderPassSetRenderTargetCommandAllocator.ReleaseInstance(static_cast<RenderPassSetRenderTargetCommand*>(renderPassCommand));
		break;
	}


}

Quad::RenderPassCommandFactory::RenderPassCommandFactory()
	:mTextureCopyCommandAllocator(200), mTextureBindCommandAllocator(200),mResourceStateTransitionCommandAllocator(200),
	mRenderPassSetRenderTargetCommandAllocator(200)
{

	mTextureCopyCommandAllocator.Initialize();
	mTextureBindCommandAllocator.Initialize();
	mResourceStateTransitionCommandAllocator.Initialize();
	mRenderPassSetRenderTargetCommandAllocator.Initialize();
}

Quad::RenderPassResourceStateTransitionCommand::RenderPassResourceStateTransitionCommand()
	:RenderPassCommand(ERenderPassCommandType::eResourceStateTransition),mTextureLocalID(-1),mTextureUniqueID(0), mTexture(nullptr)
{
}

void Quad::RenderPassResourceStateTransitionCommand::SetStateTransition(D3D12_RESOURCE_STATES sourceState, D3D12_RESOURCE_STATES destState)
{
	mSourceState = sourceState;
	mDestState = destState;
}

D3D12_RESOURCE_STATES Quad::RenderPassResourceStateTransitionCommand::GetStateBefore() const
{
	return mSourceState;
}

D3D12_RESOURCE_STATES Quad::RenderPassResourceStateTransitionCommand::GetStateAfter() const
{
	return mDestState;
}

void Quad::RenderPassResourceStateTransitionCommand::SetTextureLocalID(int localID)
{
	mTextureLocalID = localID;
}

int Quad::RenderPassResourceStateTransitionCommand::GetTextureLocalID() const
{
	return mTextureLocalID;
}

void Quad::RenderPassResourceStateTransitionCommand::SetTextureUniqueID(unsigned long long id)
{
	mTextureUniqueID = id;
}

void Quad::RenderPassResourceStateTransitionCommand::SetTexture(Texture* texture)
{
	mTexture = texture;
}

Quad::Texture* Quad::RenderPassResourceStateTransitionCommand::GetTexture() const
{
	return mTexture;
}

Quad::RenderPassSetRenderTargetCommand::RenderPassSetRenderTargetCommand()
	:RenderPassCommand(ERenderPassCommandType::eRenderTargetSetting),	mTextureLocalID(-1),mTexture(nullptr), mDepthStencilLocalID(-1), mDepthStencilBuffer(nullptr), mRenderTargetUsage(true),mDepthStencilBufferUsage(true)
{
}

void Quad::RenderPassSetRenderTargetCommand::SetRenderTargetTextureLocalID(int textureLocalID)
{
	mTextureLocalID = textureLocalID;

}

int Quad::RenderPassSetRenderTargetCommand::GetRenderTargetTextureLocalID() const
{
	return mTextureLocalID;
}

void Quad::RenderPassSetRenderTargetCommand::SetDepthStencilBufferLocalID(int dsBufferLocalID)
{
	mDepthStencilLocalID = dsBufferLocalID;
}

int Quad::RenderPassSetRenderTargetCommand::GetDepthStencilBufferLocalID() const
{
	return mDepthStencilLocalID;
}

void Quad::RenderPassSetRenderTargetCommand::SetRenderTargetTexture(RenderTargetTexture* texture)
{
	mTexture = texture;
}

Quad::RenderTargetTexture* Quad::RenderPassSetRenderTargetCommand::GetRenderTargetTexture() const
{
	return mTexture;
}

void Quad::RenderPassSetRenderTargetCommand::SetDepthStencilBuffer(Texture* dsBuffer)
{
	mDepthStencilBuffer = dsBuffer;

}

Quad::Texture* Quad::RenderPassSetRenderTargetCommand::GetDepthStencilBuffer() const
{
	return mDepthStencilBuffer;
}

void Quad::RenderPassSetRenderTargetCommand::SetRenderTargetUsage(bool flag)
{
	mRenderTargetUsage = flag;
}

void Quad::RenderPassSetRenderTargetCommand::SetDepthStencilBufferUsage(bool flag)
{
	mDepthStencilBufferUsage = flag;
}

bool Quad::RenderPassSetRenderTargetCommand::GetRenderTargetUsage() const
{
	return mRenderTargetUsage;
}

bool Quad::RenderPassSetRenderTargetCommand::GetDepthStencilBufferUsage() const
{
	return mDepthStencilBufferUsage;
}
