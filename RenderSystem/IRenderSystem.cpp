#include "IRenderSystem.h"

Render::IRenderSystem* Render::IRenderSystem::mImpl = nullptr;

Render::IRenderSystem::~IRenderSystem()
{
}

void Render::IRenderSystem::SetRenderSystemImpl(IRenderSystem* pImpl)
{
	mImpl = pImpl;

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