#include "GpuBufferContextSystemImpl.h"
#include<assert.h>	

#include<IGpuResourceManager.h>
#include<sstream>
#include<Logger.h>
Core::GpuBufferContextSystemImpl::GpuBufferContextSystemImpl()
{
	mGpuResourceManager = GRM::IGpuResourceManager::GetInstance();

}

Core::GpuBufferContextSystemImpl::~GpuBufferContextSystemImpl()
{
}

bool Core::GpuBufferContextSystemImpl::CreateGpuBuffer(uint32_t id, const GRM::BufferDesc& bufferDesc)
{

	//GpuBuffer를 생성요청 
	//타입에 맞게 생성해야한다.
	//또한 공간이 부족하면 추가로 다시 사이즈를 확장할수있게 요청하는 메서드도 제공해야한다.

	
	std::unordered_map<uint32_t, std::unique_ptr<GpuBufferContext>>::iterator it = mGpuBufferContextTable.find(id);

	if (it != mGpuBufferContextTable.end())
	{
		LOG_MESSAGE_CRITICAL("GpuBufferContextSystemImpl", "CreateGpuBffer 호출 실패(이미 존재하는 buffer id)");
		assert(0);
	}


	GRM::GRMPtr pBuffer =	mGpuResourceManager->CreateBuffer(bufferDesc);
	
	if (pBuffer.getResource() == nullptr)
	{
		LOG_MESSAGE_CRITICAL("GpuBufferContextSystemImpl", "Fun : CreaetGpuBuffer , GpuBuffer리소스 생성실패");
		assert(0);
	}


	mGpuBufferContextTable[id]->mGpuBuffer = pBuffer;
	mGpuBufferContextTable[id]->mID = id;

	return true;
}

bool Core::GpuBufferContextSystemImpl::RegisterGpuBufferCreateDataCallback(uint32_t id, pDataMemory(*createBufferData)(void* object, void* frameContext))
{

	std::unordered_map<uint32_t,std::unique_ptr<GpuBufferContext>>::iterator it	= mGpuBufferContextTable.find(id);

	if(it == mGpuBufferContextTable.end())
	{
		
		std::stringstream errorMessage;
		errorMessage << id << "의 ID를 가진 버퍼가 존재하지않습니다. 먼저 CreateGpuBuffer를 호출해야합니다.";
		LOG_MESSAGE_CRITICAL("GpuBufferContextSystemImpl", errorMessage.str().c_str());
		assert(0);
		return false;
	}

	
	mGpuBufferContextTable[id]->createBufferData = createBufferData;

	return true;

}

Core::GpuBufferContext * Core::GpuBufferContextSystemImpl::GetGpuBufferContext(uint32_t id) const
{

	std::unordered_map<uint32_t, std::unique_ptr<GpuBufferContext>>::const_iterator it = mGpuBufferContextTable.find(id);

	
	if(it == mGpuBufferContextTable.end())
	{
		std::stringstream errorMessage;
		errorMessage << id << "의 ID를 가진 버퍼가 존재하지않습니다. 먼저 CreateGpuBuffer를 호출해야합니다.";
		LOG_MESSAGE_CRITICAL("GpuBufferContextSystemImpl", errorMessage.str().c_str());
		assert(0);
		return nullptr;
	}

	return it->second.get();
}
