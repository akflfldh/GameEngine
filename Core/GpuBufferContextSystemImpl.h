#pragma once


#include"GpuBufferContextSystem.h"
#include<unordered_map>
#include<memory>

namespace Core
{

	class GpuBufferContextSystemImpl :public GpuBufferContextSystem
	{

	public:
		GpuBufferContextSystemImpl();
		virtual ~GpuBufferContextSystemImpl();



		//이 메서드는 shaderImporter가 호출할것이이다
		virtual bool CreateGpuBuffer(uint32_t, const GRM::BufferDesc& bufferDesc) override;

		
		//이메서드는 에디터와, 유저 DLL의 버퍼콜백등록 전역함수가 호출한다. 
		virtual bool RegisterGpuBufferCreateDataCallback(uint32_t id,
			pDataMemory(*createBufferData)(void* object, void* frameContext)) override;
	


		virtual GpuBufferContext* GetGpuBufferContext(uint32_t id) const override;




	private:
		GRM::IGpuResourceManager* mGpuResourceManager;
		std::unordered_map<uint32_t, std::unique_ptr<GpuBufferContext>> mGpuBufferContextTable;





	};

}