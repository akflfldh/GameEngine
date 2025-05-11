#pragma once

#include"Allocator/PoolAllocator.h"
#include<type_traits>
#include"Allocator/AllocatorDllExport.h"
namespace Quad
{
	template<typename T>
	class DestructingPoolAllocator :public PoolAllocator<T>
	{
	public:
		DestructingPoolAllocator(unsigned int elementMaxNum);
		~DestructingPoolAllocator();

		void Initialize();
		T* GetInstance();
		void ReleaseInstance(T* ptr);

	private:








	};

	template<typename T>
	inline DestructingPoolAllocator<T>::DestructingPoolAllocator(unsigned int elementMaxNum)
		:PoolAllocator<T>(elementMaxNum)
	{




	}

	template<typename T>
	inline DestructingPoolAllocator<T>::~DestructingPoolAllocator()
	{
		//소멸자 호출해줘야함


	}

	template<typename T>
	inline void DestructingPoolAllocator<T>::Initialize()
	{

		PoolAllocator<T>::Initialize();


	}

	template<typename T>
	inline T* DestructingPoolAllocator<T>::GetInstance()
	{


		/*T* pStartAddr = this->GetStartAddr();
		std::stack<T*> & pointerStack =this->GetPointerStack();
		
		if (pointerStack.empty())
			return nullptr;

		T * newAddr =	pointerStack.top();

		pointerStack.pop();*/

		T* newAddr = PoolAllocator<T>::GetInstance();


		T* newInstance = new (newAddr) T();

		return newInstance;


	}

	template<typename T>
	inline void DestructingPoolAllocator<T>::ReleaseInstance(T* ptr)
	{
		if (ptr == nullptr)
			return;

		if (std::is_class<T>::value)
		{
			ptr->~T();
		}


		PoolAllocator<T>::ReleaseInstance(ptr);

	/*	std::stack<T*>& pointerStack = this->GetPointerStack();
		pointerStack.push(ptr);*/


	}

}



