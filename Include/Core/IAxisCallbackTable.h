#pragma once


#include<unordered_map>
#include<vector>
#include<memory>

namespace Quad
{
	class IAxisCallbackTable
	{
	public:
		IAxisCallbackTable();
		virtual ~IAxisCallbackTable() = 0;

		virtual void InvokeCallback(float) = 0;

	private:


	};


	template<typename T>
	class AxisCallbackTable :public IAxisCallbackTable
	{
	public:
		AxisCallbackTable();
		virtual ~AxisCallbackTable();

		virtual void InvokeCallback(float) override;
		void Register(T*, void(T::* callback)(float));
		void Release(T*, void(T::* callback)(float));

	private:
		//중복되면 true
		bool CheckDuplicate(T*, void(T::*)(float)) const;


	private:
		std::vector<std::pair<T*, void(T::*)(float)>> mCallbackList;



	};

	template<typename T>
	inline AxisCallbackTable<T>::AxisCallbackTable()
	{
	}

	template<typename T>
	inline AxisCallbackTable<T>::~AxisCallbackTable()
	{
	}

	template<typename T>
	inline void AxisCallbackTable<T>::InvokeCallback(float value)
	{
		for (auto& element : mCallbackList)
		{
			T* object = element.first;
			void(T:: * callback)(float) = element.second;
			(object->*callback)(value);
		}

	}

	template<typename T>
	inline void AxisCallbackTable<T>::Register(T* object, void(T::* callback)(float))
	{
		if (CheckDuplicate(object, callback))
			return;

		mCallbackList.push_back({ object,callback });


	}

	template<typename T>
	inline void AxisCallbackTable<T>::Release(T* object, void(T::* callback)(float))
	{
		using pairType = std::pair<T*, void(T::*)(float)>;

		typename  std::vector<pairType>::iterator it = std::find(mCallbackList.begin(), mCallbackList.end(), pairType{ object, callback });

		if (it == mCallbackList.end())
			return;

		mCallbackList.erase(it);

	}

	template<typename T>
	inline  bool AxisCallbackTable<T>::CheckDuplicate(T* object, void(T::* callback)(float)) const
	{
		using pairType = std::pair<T*, void(T::*)(float)>;

		typename std::vector<pairType>::const_iterator it = std::find(mCallbackList.begin(), mCallbackList.end(), pairType{ object, callback });


		return it != mCallbackList.end() ? true : false;
	}

}