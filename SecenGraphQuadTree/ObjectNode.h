#pragma once

#include"Predeclare.h"


#include"SceneNode.h"
#include"ObjectSmartPointer.h"



namespace Quad
{
	class ObjectNode
	{
	public:


		void SetObject(Object* object);
		void SetObject(const ObjectSmartPointer & objectSmartPointer);
		Object* GetObject();
		


	private:
		ObjectSmartPointer mObjectSmartPointer;


	};


}