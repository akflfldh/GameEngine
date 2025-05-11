#include "Event/ResizeEvent.h"

Quad::ResizeHandleEvent::ResizeHandleEvent(const std::string& name)
	:Event(name),mNewSize({0,0,0})
{

}

void Quad::ResizeHandleEvent::SetNewSize(DirectX::XMFLOAT3 size)
{
	mNewSize = size;
}

DirectX::XMFLOAT3 Quad::ResizeHandleEvent::GetNewSize() const
{
	return mNewSize;
}

void Quad::ResizeHandleEvent::SetDirection(int dir)
{
	mDirection = dir;
}

int Quad::ResizeHandleEvent::GetDirection() const
{
	return mDirection;
}

