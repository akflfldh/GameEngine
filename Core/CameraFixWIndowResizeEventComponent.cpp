#include "CameraFixWindowResizeEventComponent.h"
#include"Event.h"
#include"WindowResizeEvent.h"

#include"Object/Camera/OrthogoanlCamera.h"
#include<cassert>

Quad::CameraFixWindowResizeEventComponent::CameraFixWindowResizeEventComponent()
	:mVerticalLineDir(EVerticalLineDir::eLeft),mHorizontalLineDir(EHorizontalLineDir::eTop)
	, mVerticalLineOrigine(0), mHorizontalLineOrigine(0),mDestCamera(0)
{


}
Quad::CameraFixWindowResizeEventComponent::~CameraFixWindowResizeEventComponent()
{


}

void Quad::CameraFixWindowResizeEventComponent::Initialize(OrthogoanlCamera* destCamera)
{
	if (destCamera == nullptr)
		assert("CameraFixWindowResizeEventComponent에 destCamera로 nullptr이 들어옴\n");

	mDestCamera = destCamera;

	AdjustCameraPos();
}

void Quad::CameraFixWindowResizeEventComponent::SetLineDir(EVerticalLineDir verticalLine, EHorizontalLineDir horizontalLine)
{

	mVerticalLineDir = verticalLine;
	mHorizontalLineDir = horizontalLine;


	AdjustCameraPos();

}

void Quad::CameraFixWindowResizeEventComponent::SetVerticalLineOrigin(float origine)
{
	mVerticalLineOrigine = origine;

	AdjustCameraPos();

}

void Quad::CameraFixWindowResizeEventComponent::SetHorizontalLineOrigin(float origine)
{
	mHorizontalLineOrigine = origine;


	AdjustCameraPos();
}

void Quad::CameraFixWindowResizeEventComponent::OnEvent(Event* pEvent)
{
	const std::string & eventName =	pEvent->GetName();

	if (eventName != "WindowResizeEvent")
		return;


	WindowResizeEvent* resizeEvent = static_cast<WindowResizeEvent*>(pEvent);

	float windowWidth =	resizeEvent->GetWindowWidth();
	float windowHeight = resizeEvent->GetWindowHeight();

	mDestCamera->SetViewWidthAndHeight(windowWidth, windowHeight);

	AdjustCameraPos();

	


}

void Quad::CameraFixWindowResizeEventComponent::AdjustCameraPos()
{


	float cameraViewWidth = mDestCamera->GetViewWidth();
	float cameraViewHeight = mDestCamera->GetViewHeight();


	float posWorldX = mVerticalLineOrigine;
	float posWorldY = mHorizontalLineOrigine;

	if (mVerticalLineDir == EVerticalLineDir::eLeft)
	{
		posWorldX += cameraViewWidth / 2;
	}
	else
	{
		posWorldX -= cameraViewWidth / 2;
	}


	if (mHorizontalLineDir == EHorizontalLineDir::eTop)
	{

		posWorldY -= cameraViewHeight / 2;
	}
	else
	{
		posWorldY += cameraViewHeight / 2;
	}
	DirectX::XMFLOAT3 beforePosW = mDestCamera->GetTransform().GetPositionWorld();

	beforePosW.x = posWorldX;
	beforePosW.y = posWorldY;
	mDestCamera->SetPositionWorld(beforePosW);
}
