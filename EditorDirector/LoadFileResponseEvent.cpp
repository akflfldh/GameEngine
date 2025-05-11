#include "LoadFileResponseEvent.h"

Quad::LoadFileResponseEvent::LoadFileResponseEvent()
	:Event("LoadFileResponse")
{
}

void Quad::LoadFileResponseEvent::SetAssetVector(const std::vector<Asset*>& assetVector)
{
	mAssetVector = assetVector;
}

void Quad::LoadFileResponseEvent::SetAssetVector(std::vector<Asset*>&& assetVector)
{
	mAssetVector = std::move(assetVector);
}

const std::vector<Quad::Asset*>& Quad::LoadFileResponseEvent::GetAssetVector() const
{
	return mAssetVector;
	// TODO: 여기에 return 문을 삽입합니다.
}
