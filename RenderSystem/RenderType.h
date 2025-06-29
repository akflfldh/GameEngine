#pragma once


#include<memory>
#include<vector>

#define D3DX

#ifdef D3DX
#include<Windows.h>
namespace Render { using WindowHandle = HWND; }
namespace Render{using }

#endif



namespace GRM { class IGpuResource; }

namespace Render
{

	//렌더시스템에 새로운채널을 생성하기위해 전달하는 구조체
	struct CreationRenderChannelInfo
	{
		WindowHandle mWindowHandle;
		float mLeft = 0.0f;
		float mTop=0.0f;
		float mWidth = 100.0f;
		float mHeight = 100.0f;
		float mMinZ = 0.0f;
		float mMaxZ = 0.0f;

		size_t mCameraID = 0;

	};








	
	struct MeshItem
	{
		size_t mVertexNum = 0;
		size_t mIndexNum = 0;
		size_t mVertexOffset = 0;
		size_t mIndexOffset = 0;
		GRM::IGpuResource* mVertexBuffer=nullptr;
		GRM::IGpuResource* mIndexBuffer =nullptr; //만약 indexBuffer nullptr이면 vertex만 사용한 draw수행
	};

	struct InstanceItem
	{
		GRM::IGpuResource* mInstanceDataBuffer = nullptr;
		size_t mInstanceCount = 1;      // 이 드로우 콜에서 그릴 인스턴스 개수 (1이면 비-인스턴싱)
		size_t mInstanceBufferOffset = 0;
		


	};

	struct RenderItem
	{ 
		MeshItem mMeshItem;
		
		
	

	
	
	};
	




}
