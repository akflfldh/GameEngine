#include "Core/WindowResizeEvent.h"

Quad::WindowResizeEvent::WindowResizeEvent()
	:Event("WindowResizeEvent")
{
}

void Quad::WindowResizeEvent::SetPreWindowSize(UINT width, UINT height)
{

	mPreWindowWidth = width;
	mPreWindowHeight = height;

}

void Quad::WindowResizeEvent::SetWindowSize(UINT width, UINT height)
{
	mWindowWidth = width;
	mWindowHeight = height;
}

UINT Quad::WindowResizeEvent::GetPreWindowWidth() const
{
	return mPreWindowWidth;
}

UINT Quad::WindowResizeEvent::GetPreWindowHeight() const
{
	return mPreWindowHeight;
}

UINT Quad::WindowResizeEvent::GetWindowWidth() const
{
	return mWindowWidth;
}

UINT Quad::WindowResizeEvent::GetWindowHeight() const
{
	return mWindowHeight;
}
