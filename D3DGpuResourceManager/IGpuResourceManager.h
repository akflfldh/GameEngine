#pragma once


#include"IGpuResource.h"

namespace GRM
{

	enum class EBufferUsage
	{
		eDefault = 0,          // 일반 GPU 리소스 (DEFAULT)
		eVertexBuffer,         // 정점 버퍼 
		eIndexBuffer,          // 인덱스 버퍼
		eConstantBuffer,       // 상수 버퍼
	};

	enum class EBufferMemoryAccess
	{
		eGpuOnly=0,		//GPU 전용(default 힙)
		eCpuWriteOnly	//CPU 쓰기(upload힙)
	};

	//상수버퍼: ElementDatasSIZE상수버퍼하나에들어가는데이터크기 -> 정렬
	//ElementDataNum은 연속된 상수버퍼의 개수
	//mData를 nullptr로 초반에 넘긴다.
	//상수버퍼의 경우에는 연속된것으로 제공하자


	struct BufferDesc
	{ 
		char* mData = nullptr; //데이터
		size_t mElementDataNum = 0; //데이터요소의수			//vertexbuffer의경우 vertex의 수
		size_t mElementDataSize = 0;//한 데이터요소의 크기		//vertexbuffer의 경우 vertex하나의 크기

		size_t mBufferSize = 0;	//buffer의최종크기



		EBufferUsage mBufferUsage = EBufferUsage::eDefault;
		EBufferMemoryAccess mBufferMemoryAccess = EBufferMemoryAccess::eGpuOnly;
	};






	


	
	class IGpuResourceManager
	{
	public:
	
		virtual ~IGpuResourceManager() = 0;
	

		virtual IGpuResource* CreateBuffer(const BufferDesc & bufferDesc) = 0;

		virtual IGpuResource* Create2DTextureResource() = 0;

		static IGpuResourceManager* GetInstance();
		static void SetGpuResourceManagerImpl(IGpuResourceManager* pImpl);

		virtual void RegisterSwapChainBackBufferView(void* resoure) = 0;
		virtual void ReleaseSwapChainBackBufferView(void* resoure) = 0;



	protected:
		IGpuResourceManager();

	private:
	
		static IGpuResourceManager* mImplManager;
	};

}
