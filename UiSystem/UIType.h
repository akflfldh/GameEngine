#pragma once

#include<stdint.h>

namespace UI
{
	using UIChannelID = uint32_t;
	using UICanvasID = uint32_t;
	using UIElementID = uint32_t;

#define InvaildUIChannelID 0
#define InvaildUICanvasID 0
#define InvaildUIElementID 0

	enum class ECanvasSizeMode
	{
		//고정된 사이즈
		eFixSize = 0,

		//스크린사이즈(스크린의 사이즈가 변하면 같이변하는 모드 ) 
		eScreenSize

	};
}