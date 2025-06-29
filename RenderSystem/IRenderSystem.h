#pragma once


#include"RenderType.h"

namespace Render
{

	class IRenderSystem
	{
	public:
		virtual ~IRenderSystem();


		static IRenderSystem* GetInstance();
		static void SetGpuResourceManagerImpl(IRenderSystem* pImpl);


		virtual int RegisterRenderChannel(const CreationRenderChannelInfo& creationChannelInfo) = 0 ;


	protected:
		IRenderSystem();


	private:
		static IRenderSystem* mImpl;




	};

}