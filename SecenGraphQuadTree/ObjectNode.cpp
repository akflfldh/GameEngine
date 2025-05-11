#include "ObjectNode.h"

void Quad::ObjectNode::SetObject(Object* object)
{
	mObjectSmartPointer = object;

}

void Quad::ObjectNode::SetObject(const ObjectSmartPointer& objectSmartPointer)
{
	mObjectSmartPointer = objectSmartPointer;
}

Quad::Object* Quad::ObjectNode::GetObject()
{
	return mObjectSmartPointer.GetPointer();
}
