
#pragma once


namespace Quad
{
	const int kMouseMoveX = 2000;
	const int kMouseMoveY = 2001;

	enum class EInputState
	{
		eKeyDown = 0,
		eKeyUp,
		eKeyHeld
	};



	struct KeyHeldStruct
	{
		int key = 0;
		float mCurrTime = 0.0f;
		float mIntervalTime = 0.2f;
	};


	struct InputContext
	{
		EInputState mState = EInputState::eKeyDown;
		int key = 0;

	};

	struct InputContextCallbackStruct
	{
		void* object;
		void(*callback)(void * object, const InputContext& inputContext);


		bool operator==(const InputContextCallbackStruct& callbackStruct)
		{
			if (object != callbackStruct.object || callback != callbackStruct.callback)
			{
				return false;
			}

			return true;
		}
	};


	template<typename T, void(T::* memfun)(const InputContext & context)>
	void staticInputContextCallback(void* object , const InputContext& inputContext)
	{
		T* objectT = static_cast<T*>(object);
		(objectT->*memfun)(inputContext);
	}



	struct CallbackStruct
	{
		void* object;
		void (*callback)(void*);

		bool operator==(const CallbackStruct& callbackStruct)
		{
			if (object != callbackStruct.object || callback != callbackStruct.callback)
			{
				return false;
			}

			return true;
		}
	};


	template<typename T, void(T::* memfun)()>
	void staticCallback(void* object)
	{
		T* objectT = static_cast<T*>(object);
		(objectT->*memfun)();
	}



	struct AxisCallbackStruct
	{
		void* object;
		void (*callback)(void*, float);

		bool operator==(const AxisCallbackStruct& callbackStruct)
		{
			if (object != callbackStruct.object || callback != callbackStruct.callback)
			{
				return false;
			}

			return true;
		}
	};


	template<typename T, void(T::* memfun)(float)>
	void staticAxisCallback(void* object, float value)
	{
		T* objectT = static_cast<T*>(object);

		(objectT->*memfun)(value);

	}

}