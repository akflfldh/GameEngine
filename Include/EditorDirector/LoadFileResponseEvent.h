#pragma once


#include"Predeclare.h"
#include"Core/Event.h"

#include<vector>

namespace Quad
{

	class LoadFileResponseEvent:public Event
	{
	public:
		LoadFileResponseEvent();
		virtual ~LoadFileResponseEvent()=default;

		void SetAssetVector(const std::vector<Asset*>& assetVector);
		void SetAssetVector(std::vector<Asset*>&& assetVector);

		const std::vector<Asset*> & GetAssetVector() const;

	private:
		std::vector<Asset*> mAssetVector;



	};
}

