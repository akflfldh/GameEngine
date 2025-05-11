#include "Object/ObjectSmartPointer.h"

#include"Object/Object.h"
#include"Utility/UniqueIDTable.h"
#include"ObjectManager/ObjectManager.h"


Quad::ObjectSmartPointer::ObjectSmartPointer()
	:mObjectID(0),mObject(nullptr)
{

}

Quad::ObjectSmartPointer::~ObjectSmartPointer()
{

}

Quad::ObjectSmartPointer::ObjectSmartPointer(const ObjectSmartPointer& pointer)
	:mObjectID(pointer.mObjectID),mObject(pointer.mObject)
{
	
}

Quad::ObjectSmartPointer::ObjectSmartPointer(Object* object)
{
	
	mObject = object;
	if (object == nullptr)
		mObjectID = 0;
	else
		mObjectID = object->GetUniqueID();

}

Quad::ObjectSmartPointer::ObjectSmartPointer(unsigned long long objectID)
{


	mObjectID = objectID;

	if (mObjectID == 0)
		mObject = nullptr;
	else
	{
		BaseObjectManager* objectManager = EditObjectManager::GetInstance();
		mObject = objectManager->GetObjectFromID(objectID);
	}



}

Quad::ObjectSmartPointer& Quad::ObjectSmartPointer::operator=(const ObjectSmartPointer& pointer)
{

	mObject = pointer.mObject;
	mObjectID = pointer.mObjectID;

	return *this;
}

Quad::ObjectSmartPointer& Quad::ObjectSmartPointer::operator=(Object* object)
{
	mObject = object;
	if (mObject == nullptr)
		mObjectID = 0;
	else
		mObjectID = mObject->GetUniqueID();


	return *this;
}

Quad::ObjectSmartPointer& Quad::ObjectSmartPointer::operator=(unsigned long long id)
{

	BaseObjectManager* objectManager = EditObjectManager::GetInstance();
	mObjectID = id;
	
	mObject = objectManager->GetObjectFromID(mObjectID);

	return *this;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::Object* Quad::ObjectSmartPointer::operator->() const
{
	return mObject;
}

Quad::Object* Quad::ObjectSmartPointer::GetPointer(bool checkTableFlag) 
{

	if (checkTableFlag)
	{	
		CheckAddress();
	}

	return mObject;
}

Quad::Object* Quad::ObjectSmartPointer::GetPointer() const
{
	return mObject;
}

unsigned long long Quad::ObjectSmartPointer::GetObjectID() const
{

	
	return mObjectID;
}

void Quad::ObjectSmartPointer::CheckAddress()
{
	

	if (mObjectID == 0)
		mObject = nullptr;
	else
	{
		BaseObjectManager* objectManager = EditObjectManager::GetInstance();
		mObject = objectManager->GetObjectFromID(mObjectID);
	}


}
