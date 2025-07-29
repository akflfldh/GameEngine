#include "pch.h"
#include "ISystemInitializer.h"
#include<assert.h>
SystemInitializer::ISystemInitializer* SystemInitializer::ISystemInitializer::mImpl =nullptr;



SystemInitializer::ISystemInitializer::~ISystemInitializer()
{
}

void SystemInitializer::ISystemInitializer::SetSystemInitializerImpl(ISystemInitializer* pImpl)
{
	if (pImpl == nullptr || mImpl !=nullptr)
	{
		assert(0);
	}



	mImpl = pImpl;

}

SystemInitializer::ISystemInitializer* SystemInitializer::ISystemInitializer::GetInstance()
{
	if (mImpl == nullptr)
	{
		assert(0);
	}


	return mImpl;

}

SystemInitializer::ISystemInitializer::ISystemInitializer()
{

}
