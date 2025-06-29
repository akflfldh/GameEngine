#pragma once


namespace Quad
{
	enum class  EInputState
	{
		eKeyDown = 0,
		eKeyUp,
		eKeyHeld
	};

	struct  KeyHeldStruct
	{
		int key = 0;
		float mCurrTime = 0.0f;
		float mIntervalTime = 0.2f;
	};


	enum class EMouseInput
	{
		eLButtonDown =0 ,
		eLButtonUp,
		eRButtonDown,
		eRButtonUp,
		eMouseMove
	};

	struct MouseInputData
	{
		EMouseInput mMouseInputType;

		int mClientPositionX;
		int mClientPositionY;
		int mScreenPositionX;
		int mScreenPositionY;


		int mPreClientPositionX;
		int mPreClientPositionY;
		int mPreScreenPositionX;
		int mPreScreenPositionY;

	};



}