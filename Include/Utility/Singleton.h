#pragma once

#include<cassert>


#ifdef UTILITY_LIB_EXPORT
#define UTILITY_LIB_API __declspec(dllexport)
#else
#define UTILITY_LIB_API __declspec(dllimport)
#endif

namespace Quad
{
	template<typename T>
	class Singleton
	{
	public:
		Singleton();
		~Singleton();


		static T* GetInstance();
	private:
		static T* gInstance;



	};

	template<typename T>
	T* Singleton<T>::gInstance = nullptr;


	template<typename T>
	Singleton<T>::Singleton()
	{

		assert(!gInstance);
		gInstance = (T*)this;

	}

	template<typename T>
	Singleton<T>::~Singleton()
	{
		gInstance = nullptr;

	}

	template<typename T>
	T* Singleton<T>::GetInstance()
	{
		assert(gInstance);

		return gInstance;
	}

}