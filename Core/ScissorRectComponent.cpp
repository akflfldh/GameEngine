#include "ScissorRectComponent.h"
#include"Object/UiEntity.h"
#include"Map/Map.h"

#include"Object/Camera/CameraHelper.h"

#include"Object/ObjectSmartPointer.h"
#include<Component/SceneComponent.h>

#include<Component/UIMeshComponent.h>

Quad::ScissorRectComponent::ScissorRectComponent()
	:mParentRectOverlapFlag(true)
{
}

Quad::ScissorRectComponent::~ScissorRectComponent()
{
}

void Quad::ScissorRectComponent::Start()
{


}

void Quad::ScissorRectComponent::Update(float delta)
{
	if (mParentRectOverlapFlag)		//부모의 rect를 사용하니 계산할필요가없다.
		return;

	UiEntity* destObject =static_cast<UiEntity*>(GetDestObject());

	DirectX::XMFLOAT3 posW = destObject->GetRootSceneComponent()->GetTranslationWorld();

	float width = destObject->GetUiMeshComponent()->GetWidthWorld();
	float height = destObject->GetUiMeshComponent()->GetHeightWorld();		//global에대한 width ,height는 아니다.


	Map * map =	destObject->GetMap();
	int mapLayerIndex =	destObject->GetMapLayerID();

	const MapLayer & mapLayer = map->GetMapLayer(mapLayerIndex);
	D3D12_VIEWPORT viewport = mapLayer.mViewPortGlobal;
	viewport.TopLeftX = 0;	//maplayer별로 렌더타켓이 각각존재함으로 최종 viewport의 topLeftX,Y가 아닌 그 렌더타켓에 렌더링되는거니
	//0,0으로 맞춘다.

	viewport.TopLeftY = 0;
	mapLayer.mCamera;




	
	DirectX::XMFLOAT3 left = posW;
	DirectX::XMFLOAT3 right = posW;
	DirectX::XMFLOAT3 top = posW;
	DirectX::XMFLOAT3 bottom = posW;

	float wHalf = width / 2;
	float hHalf = height / 2;

	left.x -= wHalf;
	right.x += wHalf;
	top.y += hHalf;
	bottom.y -= hHalf;


	DirectX::XMFLOAT3 screenLeft,screenRight,screenTop,screenBottom;

	CameraHelper::ToScreen(left, viewport, *mapLayer.mCamera, screenLeft);
	CameraHelper::ToScreen(right, viewport, *mapLayer.mCamera, screenRight);
	CameraHelper::ToScreen(top, viewport, *mapLayer.mCamera, screenTop);
	CameraHelper::ToScreen(bottom, viewport, *mapLayer.mCamera, screenBottom);


	RECT rect;
	rect.left = screenLeft.x;
	rect.right = screenRight.x;
	rect.top = screenTop.y;
	rect.bottom = screenBottom.y;
	//setRect()
	SetRect(rect);


}

const RECT Quad::ScissorRectComponent::GetRect() const
{
	return mRect;
}




void Quad::ScissorRectComponent::SetRect(const RECT& rect)
{
	
	/*if (mParentRectOverlapFlag == false)
		return;*/


	//자식 object들에게도 전달한다.

	mRect = rect;

	Object * destObject =	GetDestObject();

	const std::vector<Object*> & childObjectVector =	destObject->GetChildObjectVector();

	for (auto & objectPtr :childObjectVector)
	{
		Object* childObject = objectPtr;
		if (childObject == nullptr)
			continue;

		ScissorRectComponent * scissorRectComponent =childObject->GetComponent<ScissorRectComponent>("ScissorRectComponent");

		if (scissorRectComponent == nullptr)
			continue;

		scissorRectComponent->SetRect(rect);
	}


}


void Quad::ScissorRectComponent::SetParentRectOverlapFlag(bool flag)
{
	mParentRectOverlapFlag = flag;
}