#pragma once

#include"SystemInitializerDll.h"
namespace SystemInitializer
{

	class SYSTEM_INITIALIZER_API ISystemInitializer
	{
	public:
		//한번만생성
		virtual ~ISystemInitializer()= 0 ;


		virtual void Initialize() = 0;
		
		static void SetSystemInitializerImpl(ISystemInitializer* pImpl); 
		static ISystemInitializer* GetInstance();
	protected:
		ISystemInitializer();

	private:
		static ISystemInitializer* mImpl;

	};
}