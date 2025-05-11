#pragma once

#include<vector>


#ifdef UTILITY_LIB_EXPORT
#define UTILITY_LIB_API __declspec(dllexport)
#else
#define UTILITY_LIB_API __declspec(dllimport)
#endif



namespace Quad
{
	template<typename T>
	class CircleQueue
	{
	public:
		CircleQueue();
		~CircleQueue() = default;		//pop될떄 소멸자가 호출되지않는다.(호출되도록한다면,따로 특수한 allocator를이용해서
		//pop할때마다 소멸자를호출하는 방식을 사용해야할거같다)

		void Initialize(size_t size);


		void PushBack(const T& element);
		const T& Front() const;
		void Pop();
		bool isEmpty()const;
		bool isFull() const;

	private:





	private:
		std::vector<T> mBuffer;
		size_t mMaxSize;
		size_t mCurrentCount;
		size_t mPushIndex;		//push할 위치
		size_t mPopIndex;		//pop할 위치




	};



	template<typename T>
	inline CircleQueue<T>::CircleQueue()
	{




	}

	template<typename T>
	inline void CircleQueue<T>::Initialize(size_t size)
	{
		mBuffer.resize(size);
		mMaxSize = size;
		mCurrentCount = 0;
		mPushIndex = 0;
		mPopIndex = 0;

	}

	template<typename T>
	inline void CircleQueue<T>::PushBack(const T& element)
	{
		if (isFull())
		{
			throw "buffer is full";
		}
		if (mPushIndex > mMaxSize)
		{
			int a = 2;
		}

		mBuffer[mPushIndex] = element;
		mPushIndex = (mPushIndex + 1) % mMaxSize;
		mCurrentCount++;
		return;

	}

	template<typename T>
	inline const T& CircleQueue<T>::Front() const
	{

		if (isEmpty())
		{
			throw "buffer is empty";
		}

		return mBuffer[mPopIndex];

		// TODO: 여기에 return 문을 삽입합니다.
	}

	template<typename T>
	inline void CircleQueue<T>::Pop()
	{

		if (isEmpty())
		{
			throw "buffer is empty";
		}
		mPopIndex = (mPopIndex + 1) % mMaxSize;
		mCurrentCount--;
		//mBuffer[mPopIndex]

	}

	template<typename T>
	inline bool CircleQueue<T>::isEmpty() const
	{
		return mCurrentCount == 0 ? true : false;
	}

	template<typename T>
	inline bool CircleQueue<T>::isFull() const
	{
		return mCurrentCount == mMaxSize ? true : false;
	}
	;


}