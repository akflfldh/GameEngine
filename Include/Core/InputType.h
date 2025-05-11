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
}