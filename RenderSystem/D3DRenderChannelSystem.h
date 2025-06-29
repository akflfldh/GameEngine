#pragma once


#include"RenderType.h"

#ifdef D3DX

#include<d3d12.h>

namespace D3DRender
{

	class D3DRenderChannelSystem
	{
	public:
		D3DRenderChannelSystem(const Render::CreationRenderChannelInfo & creationChannelInfo);
		~D3DRenderChannelSystem();


		
	private:
		HWND mWindowHandle;
		D3D12_VIEWPORT mViewport;


	};
}

#endif

