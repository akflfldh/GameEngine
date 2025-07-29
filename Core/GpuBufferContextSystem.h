#pragma once



#include<stdint.h>
#include<GRMPtr.h>
#include<GpuTypes.h>
namespace Core
{

	using pDataMemory = void* ;

	struct GpuBufferContext
	{
		uint32_t mID;
		GRM::GRMPtr mGpuBuffer;
		//type (constant(object,pass) , structured(object) )	

		//여기서 pass인경우에는 에디터는 object파라미터에 nullptr을 전달할것이고, frameContext에만 유효한 파라미터를 전달할것이다.
		// buffer타입이 object인경우에는 object파라미터와 frameContext파라미터 둘다 유효하다.
		pDataMemory(*createBufferData)(void* object, void* frameContext) =nullptr ;
	};





	class GpuBufferContextSystem
	{
	public:
		static GpuBufferContextSystem* GetInstance();
		virtual ~GpuBufferContextSystem() = 0;

		//이 메서드는 shaderImporter가 호출한다. 
		virtual bool CreateGpuBuffer(uint32_t, const GRM::BufferDesc& bufferDesc) = 0;
		
		// 이메서드는 에디터와, 유저 DLL의 전역함수가 호출한다.
		virtual bool RegisterGpuBufferCreateDataCallback(uint32_t id ,
			pDataMemory(*createBufferData)(void* object, void* frameContext)) = 0;



		//이 메서드는 renderItem Builder들이 호출할것이다.
		virtual GpuBufferContext* GetGpuBufferContext(uint32_t id) const = 0;




	protected:
		GpuBufferContextSystem();


	private:
		



	};
}

