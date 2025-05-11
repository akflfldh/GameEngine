#include "CameraEventComponentFactory.h"

#include"Allocator/BinnedAllocator.h"


constexpr int maxAlloctorNum = 10;


Quad::CameraEventComponentFactory::CameraEventComponentFactory()
	:mBinnedAllocatorVector(maxAlloctorNum,nullptr)
{
	


	mBinnedAllocatorVector[0] = new BinnedAllocator(8);
	mBinnedAllocatorVector[1] = new BinnedAllocator(16);
	mBinnedAllocatorVector[2] = new BinnedAllocator(24);
	mBinnedAllocatorVector[3] = new BinnedAllocator(48);
	mBinnedAllocatorVector[4] = new BinnedAllocator(96);
	mBinnedAllocatorVector[5] = new BinnedAllocator(128);
	mBinnedAllocatorVector[6] = new BinnedAllocator(256);
	mBinnedAllocatorVector[7] = new BinnedAllocator(512);
	mBinnedAllocatorVector[8] = new BinnedAllocator(1024);
	mBinnedAllocatorVector[9] = new BinnedAllocator(2048);



}

Quad::CameraEventComponentFactory::~CameraEventComponentFactory()
{

	for (auto& element : mBinnedAllocatorVector)
	{
		delete element;


	}


}

Quad::BinnedAllocator* Quad::CameraEventComponentFactory::GetAllocator(size_t allocSize)
{

	auto it = std::find_if(mBinnedAllocatorVector.begin(), mBinnedAllocatorVector.end(), [allocSize](const BinnedAllocator* allocator) {

		if (allocator->GetSize() >= allocSize)
			return true;

		return false;

		}
	);

	if (it == mBinnedAllocatorVector.end())
	{
		assert("CameraEventComponentFactory에 최대사이즈보다 큰 사이즈가 들어옴");
	}

	return *it;
}

