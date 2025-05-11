#include "Core/MouseClickSelectEvent.h"
#include"Core/Ray.h"

Quad::MouseClickSelectEvent::MouseClickSelectEvent(const std::string & name)
	:Event(name)
{

}

Quad::Ray Quad::MouseClickSelectEvent::GetRay() const 
{
	return mRay;
}

void Quad::MouseClickSelectEvent::SetRay(const Ray& ray)
{
	mRay = ray;
}
