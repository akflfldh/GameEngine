#include "IRenderSystem.h"

Render::IRenderSystem* Render::IRenderSystem::mImpl = nullptr;

Render::IRenderSystem::~IRenderSystem()
{
}

void Render::IRenderSystem::SetGpuResourceManagerImpl(IRenderSystem* pImpl)
{
	mImpl = pImpl;

}

bool Render::IRenderSystem::RegisterRenderChannel(const CreationRenderChannelInfo& creationChannelInfo)
{
	return false;
}

Render::IRenderSystem::IRenderSystem()
{
}


Render::IRenderSystem* Render::IRenderSystem::GetInstance()
{

	if(mImpl == nullptr)
	{ 
		//에러출력
		return nullptr;
	}

	return mImpl;

}