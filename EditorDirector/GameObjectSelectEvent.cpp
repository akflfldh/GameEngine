#include "GameObjectSelectEvent.h"

Quad::GameObjectSelectEvent::GameObjectSelectEvent()
	:Event("GameObjectSelect")
{
}

void Quad::GameObjectSelectEvent::SetSelectedObject(Object* object)
{
	mObject = object;
}

Quad::Object* Quad::GameObjectSelectEvent::GetSelectObject() const
{
	return mObject;
}

void Quad::GameObjectSelectEvent::SetCurrentSubMeshIndex(int index)
{
	mCurrentSubMeshIndex = index;
}

int Quad::GameObjectSelectEvent::GetCurrentSubMeshIndex() const
{
	return mCurrentSubMeshIndex;
}
