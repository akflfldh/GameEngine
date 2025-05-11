#pragma once

#include"Predeclare.h"


#include"Utility/Singleton.h"
#include<vector>

#include"Core/CoreDllExport.h"

namespace Quad
{
	
	class BinnedAllocator;
	class ICameraEventComponent;
	

	//현재 CameraEventComponent의 최대크기 제한은 2048바이트 
	class CORE_API_LIB CameraEventComponentFactory:public Singleton<CameraEventComponentFactory>
	{
	public:
		CameraEventComponentFactory();
		~CameraEventComponentFactory();


		template<typename componentType>
		componentType* CreateComponent();


		template<typename componentType>
		void ReleaseComponent(componentType* component);


	private:
		Quad::BinnedAllocator* GetAllocator(size_t allocSize);//할당할 사이즈보다 큰 할당자 중 가장작은 할당자 반환

		std::vector<BinnedAllocator*> mBinnedAllocatorVector;

	};

	template<typename componentType>
	inline componentType* CameraEventComponentFactory::CreateComponent()
	{
		static_assert(std::is_base_of< ICameraEventComponent, componentType>::value, "ICameraEventComponent의 파생클래스가 아닙니다.\n");


		BinnedAllocator * allocator= GetAllocator(sizeof(componentType));
		void * ptr=  allocator->Alloc();
		componentType* component = new(ptr)componentType;

		return component;
	}

	template<typename componentType>
	inline void CameraEventComponentFactory::ReleaseComponent(componentType* component)
	{
		static_assert(std::is_base_of< ICameraEventComponent, componentType>::value, "ICameraEventComponent의 파생클래스가 아닙니다.\n");

		BinnedAllocator* allocator = GetAllocator(sizeof(componentType));

		allocator->Release(component);

		return;
	}

}
