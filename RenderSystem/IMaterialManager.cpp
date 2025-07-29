#include "IMaterialManager.h"
#include<assert.h>



Render::IMaterialManager* Render::IMaterialManager::mImpl =nullptr;

Render::IMaterialManager::IMaterialManager()
{ 


	
}
Render::IMaterialManager::~IMaterialManager()
{

}

Render::IMaterialManager* Render::IMaterialManager::GetInstance()
{
	if (mImpl == nullptr)
	{
		assert(0);
	}

	return mImpl;

}


void Render::IMaterialManager::SetMaterialManagerImpl(IMaterialManager* pImpl)
{
	if (mImpl != nullptr)
	{ 
		assert(0);
	}

	mImpl = pImpl;

}