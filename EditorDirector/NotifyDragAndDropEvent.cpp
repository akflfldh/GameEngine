#include "NotifyDragAndDropEvent.h"

Quad::NotifyDragAndDropEvent::NotifyDragAndDropEvent()
	:Event("NotifyDragAndDrop")
{

}

void Quad::NotifyDragAndDropEvent::SetContentItem(ContentItem* contentItem)
{

	mContentItem = contentItem;


}

Quad::ContentItem* Quad::NotifyDragAndDropEvent::GetContentItem() const
{
	return mContentItem;
}

void Quad::NotifyDragAndDropEvent::SetScreenPosX(int x)
{
	mScreenPosX = x;
}

void Quad::NotifyDragAndDropEvent::SetScreenPosY(int y)
{
	mScreenPosY = y;
}

int Quad::NotifyDragAndDropEvent::GetScreenPosX() const
{
	return mScreenPosX;
}

int Quad::NotifyDragAndDropEvent::GetScreenPosY() const
{
	return mScreenPosY;
}

void Quad::NotifyDragAndDropEvent::SetLButtonUpState(bool state)
{
	mLButtonUp = state;
}

bool Quad::NotifyDragAndDropEvent::GetLButtonUpState() const
{
	return mLButtonUp;
}

void Quad::NotifyDragAndDropEvent::SetRay(const Ray& ray)
{
	mRay = ray;
}

const Quad::Ray& Quad::NotifyDragAndDropEvent::GetRay() const
{
	return mRay;
	// TODO: 여기에 return 문을 삽입합니다.
}
