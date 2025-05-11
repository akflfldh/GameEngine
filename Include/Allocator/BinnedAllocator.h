#pragma once

#include"Predeclare.h"
#include<vector>


#include"Allocator/AllocatorDllExport.h"



namespace Quad
{


	class ALLOCATOR_API_LIB BinnedAllocator
	{
		

	public:
		BinnedAllocator(unsigned int size);
		~BinnedAllocator();

		void* Alloc();
		void Release(void* ptr);

		const unsigned int GetSize() const;


	private:
	


		unsigned int mSize;

		struct Chunk
		{
			Chunk(unsigned int size);
			~Chunk();
			char* mStartAddr;
		};

		std::vector<Chunk*> mChunkVector;

		char* mHeadPtr;
		

	};


	inline BinnedAllocator::BinnedAllocator(unsigned int size)
		:mSize(size)
	{
		mChunkVector.push_back(new Chunk(mSize));
		mHeadPtr = mChunkVector[0]->mStartAddr;
	}


	inline BinnedAllocator::~BinnedAllocator()
	{
		for (auto element : mChunkVector)
		{
			delete element;
		}

	}

	


	inline void* BinnedAllocator::Alloc()
	{
		void* test = new char[mSize];
		return test;




		//auto instance = BinnedAllocator::GetInstance();
		void* retPtr = mHeadPtr;
		std::vector<Chunk*>& chunkVector = mChunkVector;
		if (*(uintptr_t*)retPtr == 0)
		{
			//새로운 chunk 할당
			chunkVector.push_back(new Chunk(mSize));
			retPtr = chunkVector[chunkVector.size() - 1]->mStartAddr;
		}

		uintptr_t nextPtr = *(uintptr_t*)retPtr;
		mHeadPtr =(char*)nextPtr;

		return retPtr;
	}


	inline void BinnedAllocator::Release(void * ptr)
	{

		if (ptr == nullptr)
			return;

		delete[] ptr;
		return;

		//auto instance = BinnedAllocator<size>::GetInstance();
		
		*(uintptr_t*)ptr = (uintptr_t)mHeadPtr;
		mHeadPtr = (char*)ptr;


	}

	inline const unsigned int BinnedAllocator::GetSize() const
	{
		return mSize;
	}







	inline BinnedAllocator::Chunk::Chunk(unsigned int size)
	{
		mStartAddr = new char[size * 1000 + 16];
		char shift  = 16- (uintptr_t)mStartAddr % 16;
		mStartAddr += shift;

		*(mStartAddr - 1) = shift;

		char* addr = mStartAddr;
		for (int i = 0; i < 1000 - 1; ++i)
		{
			*(uintptr_t*)addr = (uintptr_t)(addr + size);
			addr += size;
		}
		*(uintptr_t*)addr = 0;

	}

	inline BinnedAllocator::Chunk::~Chunk()
	{

		char shift = *(char*)(mStartAddr - 1);
		mStartAddr -= shift;

		delete[] mStartAddr;
	}

}
