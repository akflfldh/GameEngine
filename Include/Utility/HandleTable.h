#pragma once

#include"Predeclare.h"
#include<stack>
#include<vector>
#include"Singleton.h"
#include"Windows.h"



#ifdef UTILITY_LIB_EXPORT
#define UTILITY_LIB_API __declspec(dllexport)
#else
#define UTILITY_LIB_API __declspec(dllimport)
#endif

namespace Quad
{
	typedef unsigned long long QHANDLE;
	
	template<typename valueType>
	class HandleTable
	{
	public:
		HandleTable(int elementMaxNum, valueType initValue);
		~HandleTable();

		HandleTable(const HandleTable& table);
		HandleTable(HandleTable&& table);

		HandleTable<valueType>& operator=(const HandleTable& table);
		HandleTable<valueType>& operator=(HandleTable&& table);

		QHANDLE Register(const valueType & value);
		void Release(QHANDLE handle);

		valueType* GetValue(QHANDLE  handle);


		const std::vector<QHANDLE>& GetUsingHandleVector() const;


	private:
		std::vector<valueType> mHandleTable;
		std::stack<QHANDLE> mFreeHandleStack;


		valueType mInitValue;
		QHANDLE mNextHandle;
		


		std::vector<QHANDLE> mUsingHandleVector;


	};


	template<typename valueType>
	inline HandleTable<valueType>::HandleTable(int elementMaxNum, valueType initValue)
		:mHandleTable(elementMaxNum,initValue),mNextHandle(0)
	{


	}

	template<typename value>
	inline HandleTable<value>::~HandleTable()
	{



	}

	template<typename valueType>
	inline HandleTable<valueType>::HandleTable(const HandleTable& table)
		:mHandleTable(table.mHandleTable.size(),table.mInitValue),mNextHandle(0)
	{
		
	}

	template<typename valueType>
	inline HandleTable<valueType>::HandleTable(HandleTable&& table)
		:mHandleTable(table.mHandleTable.size(), table.mInitValue), mHandleTable(std::move(table.mHandleTable)), mFreeHandleStack(std::move(table.mFreeHandleStack))
	{

	}

	template<typename valueType>
	inline HandleTable<valueType>& HandleTable<valueType>::operator=(const HandleTable& table)
	{

		mInitValue = table.mInitValue;
		mHandleTable.resize(table.mHandleTable.size(),mInitValue);
		mNextHandle = 0;
		// TODO: 여기에 return 문을 삽입합니다.

		return *this;
	}

	template<typename valueType>
	inline HandleTable<valueType>& HandleTable<valueType>::operator=(HandleTable&& table)
	{

		mHandleTable = std::move(table.mHandleTable);
		mFreeHandleStack = std::move(table.mFreeHandleStack);
		mNextHandle = table.mNextHandle;


		return *this;
		// TODO: 여기에 return 문을 삽입합니다.
	}

	template<typename valueType>
	inline QHANDLE HandleTable<valueType>::Register(const valueType & value)
	{

		if (!mFreeHandleStack.empty())
		{
			QHANDLE handle = mFreeHandleStack.top();
			mFreeHandleStack.pop();
			mHandleTable[handle] = value;


			mUsingHandleVector.push_back(handle);
			return handle;
		}

		mHandleTable[mNextHandle] = value;
		mUsingHandleVector.push_back(mNextHandle);
		return mNextHandle++;

	}


	template<typename valueType>
	inline void HandleTable<valueType>::Release(QHANDLE handle)
	{

		mHandleTable[handle] = mInitValue;
		mFreeHandleStack.push(handle);
		mUsingHandleVector.erase(std::remove(mUsingHandleVector.begin(), mUsingHandleVector.end(), handle), mUsingHandleVector.end());

		return;
	}

	template<typename valueType>
	inline valueType *  HandleTable<valueType>::GetValue(QHANDLE handle)
	{
		return &mHandleTable[handle];
	}

	template<typename valueType>
	inline const std::vector<QHANDLE>& HandleTable<valueType>::GetUsingHandleVector() const
	{

		return mUsingHandleVector;
		// TODO: 여기에 return 문을 삽입합니다.
	}




	struct QOBHANDLE
	{
		explicit QOBHANDLE(unsigned long long value) { mValue = value; }

		bool operator==(QOBHANDLE obHandle) { return this->mValue == obHandle.mValue ? true : false; }
		bool operator!=(QOBHANDLE obHandle) { return this->mValue == obHandle.mValue ? false : true; }

		unsigned long long mValue;
	};

	template<>
	class HandleTable<Object*> :public Singleton<HandleTable<Object*>>
	{
	public:
		HandleTable(unsigned long long maxSize= 1000000);
		~HandleTable();


		HandleTable(const HandleTable& table) = delete;
		HandleTable(HandleTable&& table) =delete;

		HandleTable<Object*>& operator=(const HandleTable<Object*>& table) =delete;
		HandleTable<Object*>& operator=(HandleTable<Object*>&& table)=delete ;


		static QOBHANDLE Register(Object* object);
		static void Release(QOBHANDLE handle);

		static Object* GetObjectPointer(QOBHANDLE  handle);

	private:
		
		std::vector<Object*> mHandleTable;
		std::stack<QOBHANDLE> mFreeHandleStack;
		QOBHANDLE mMaxHandle;
		QOBHANDLE mNextHandle;
		

	};

	inline HandleTable<Object*>::HandleTable(unsigned long long maxSize)
		:mHandleTable(maxSize,nullptr), mNextHandle(0), mMaxHandle(maxSize-1)
	{

	}

	inline HandleTable<Object*>::~HandleTable()
	{

	}

	inline QOBHANDLE HandleTable<Object*>::Register(Object* object)
	{
		auto instance = GetInstance();
		QOBHANDLE availableHandle(0);
		if (!instance->mFreeHandleStack.empty())
		{
			availableHandle = instance->mFreeHandleStack.top();
			instance->mFreeHandleStack.pop();
		}
		else
		{
			availableHandle = instance->mNextHandle;
			if (availableHandle.mValue > instance->mMaxHandle.mValue)
			{
				OutputDebugString(L"핸들테이블의 여분의핸들이없다.\n");
				assert(-1);
			}

			instance->mNextHandle.mValue++;
		}


		instance->mHandleTable[availableHandle.mValue] = object;
		//object->SetHandleValue();
		return availableHandle;
	}


	inline void HandleTable<Object*>::Release(QOBHANDLE handle)
	{
		auto instance = GetInstance();
		if (handle.mValue > instance->mMaxHandle.mValue)
			return;

		instance->mHandleTable[handle.mValue] = nullptr;
		instance->mFreeHandleStack.push(handle);

		return;
	}


	inline Object* HandleTable<Object*>::GetObjectPointer(QOBHANDLE  handle)
	{
		auto instance = GetInstance();

		if (handle.mValue > instance->mMaxHandle.mValue)
			return nullptr;

		return instance->mHandleTable[handle.mValue];

	}


}