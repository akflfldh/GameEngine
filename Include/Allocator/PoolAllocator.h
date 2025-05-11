#pragma once

#include"Utility/Utility.h"
#include<vector>
#include<stack>

#include"Allocator/AllocatorDllExport.h"

namespace Quad
{

	//T 사이즈가 8바이트 이상일것.
	template<typename T>
	class  PoolAllocator
	{
	public:
		PoolAllocator(unsigned int elementMaxNum);
		virtual ~PoolAllocator();

		virtual void Initialize();
		virtual T* GetInstance();
		virtual void ReleaseInstance(T* ptr) ;

	protected:

		void SetStartAddr(T* addr);
		T* GetStartAddr();
		size_t GetElementMaxNum() const;

	private:
		T* mStartAddr;
		size_t mElementMaxNum;

		T* mNextAddr;
		std::stack<T*> mStack;
	};

	template<typename T>
	inline PoolAllocator<T>::PoolAllocator(unsigned int elementNum)
		:mStartAddr(nullptr), mElementMaxNum(elementNum), mNextAddr(nullptr)
	{

	}
	template<typename T>
	inline PoolAllocator<T>::~PoolAllocator()
	{
		Utility::FreeAligned(GetStartAddr());

	}
	template<typename T>
	inline void PoolAllocator<T>::Initialize()
	{

		//alloc 
		T* pStartAddr = (T*)Utility::AllocAligned(this->GetElementMaxNum() * sizeof(T), alignof(T));
		this->SetStartAddr(pStartAddr);

		for (int i = 0; i < mElementMaxNum; ++i)
		{
			////pStartAddr[i] = &pStartAddr[i + 1];

			//T* nextAddr = &pStartAddr[i+1];
			////memcpy(&pStartAddr[i], &nextAddr, sizeof(T*));
			//*reinterpret_cast<T**>(&pStartAddr[i]) = nextAddr;
			mStack.push(&pStartAddr[i]);

		}
		//memset(&pStartAddr[mElementMaxNum - 1], 0, sizeof(T));
		mNextAddr = &pStartAddr[0];

		
	}
	template<typename T>
	inline T* PoolAllocator<T>::GetInstance()
	{
		T* currentAddr;
		//memcpy(&mNextAddr, mNextAddr, sizeof(mNextAddr));

		//mNextAddr = *reinterpret_cast<T**>(mNextAddr);

		currentAddr = mStack.top();
		mStack.pop();

		return currentAddr;
	}
	template<typename T>
	inline void PoolAllocator<T>::ReleaseInstance(T* ptr)
	{
		if (ptr == nullptr)
			return;

		
	//	(T*)*ptr = mNextAddr;
		//memcpy(ptr, &mNextAddr, sizeof(mNextAddr));
		
	//	*reinterpret_cast<T**>(ptr) = mNextAddr;
		//mNextAddr = ptr;
		memset(ptr, 0, sizeof(T));
		mStack.push(ptr);

		return;
	}


	template<typename T>
	inline void PoolAllocator<T>::SetStartAddr(T* addr)
	{
		mStartAddr = addr;
	}
	template<typename T>
	inline T* PoolAllocator<T>::GetStartAddr()
	{
		return mStartAddr;
	}
	template<typename T>
	inline size_t PoolAllocator<T>::GetElementMaxNum() const
	{
		return mElementMaxNum;
	}



}