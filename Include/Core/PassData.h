#pragma once

#include"Predeclare.h"


namespace Quad
{
	struct PassData
	{
		Camera* mCamera;
		std::vector<LightItem*> mDirectionalLightItemVector;
		std::vector<LightItem*> mSpotLightItemVector;
		std::vector<LightItem*> mPointSpotLightItemVector;

		D3D12_VIEWPORT mViewPort;

	};
}