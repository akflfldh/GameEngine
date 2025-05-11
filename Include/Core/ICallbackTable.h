#pragma once

#include<unordered_map>
#include<vector>
#include<utility>


namespace Quad
{


	class ICallbackTable
	{
	public:
		ICallbackTable();
		virtual ~ICallbackTable() = 0;

		virtual void InvokeCallback() = 0;

		virtual int GetTableID() = 0;

		template<typename T>
		static const int GetTableIDStaticMethod();

	protected:


	private:


		static int mCurrentID;
	};


	template<typename T>
	class CallbackTable :public ICallbackTable
	{
	public:
		CallbackTable();
		virtual ~CallbackTable();

		virtual void InvokeCallback() override;
		void Register(T*, void(T::* callback)(void));

		void Release(T*, void(T::* callback)(void));

		virtual int GetTableID() override;

	private:
		std::vector<std::pair<T*, void(T::*)(void)>> mCallbackList;

	};

	template<typename T>
	inline CallbackTable<T>::CallbackTable()
	{
	}

	template<typename T>
	inline CallbackTable<T>::~CallbackTable()
	{
	}

	template<typename T>
	inline void CallbackTable<T>::InvokeCallback()
	{
		for (auto& element : mCallbackList)
		{
			T* object = element.first;
			void (T:: * callback)(void) = element.second;
			(object->*callback)();
		}


	}

	template<typename T>
	inline void CallbackTable<T>::Register(T* object, void(T::* callback)(void))
	{
		mCallbackList.push_back({ object,callback });
	}

	template<typename T>
	inline void CallbackTable<T>::Release(T* object, void(T::* callback)(void))
	{
		std::pair<T*, void(T::*)(void)> element = { object,callback };

		typename std::vector<std::pair<T*, void(T::*)(void)>>::iterator it = std::find(mCallbackList.begin(), mCallbackList.end(), element);

		if (it != mCallbackList.end())
		{
			mCallbackList.erase(it);
		}

	}

	template<typename T>
	inline int CallbackTable<T>::GetTableID()
	{
		int id = ICallbackTable::GetTableIDStaticMethod<T>();
		return id;
	}

	template<typename T>
	const inline int ICallbackTable::GetTableIDStaticMethod()
	{
		static int currentID = mCurrentID++;
		return currentID;
	}



}