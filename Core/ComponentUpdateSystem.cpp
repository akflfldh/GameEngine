#include "ComponentUpdateSystem.h"
#include"Object/Object.h"
Quad::ComponentUpdateSystem::~ComponentUpdateSystem()
{
}

void Quad::ComponentUpdateSystem::AddObject(Object* object)
{

	size_t index = mObjectVector.size();
	mObjectVector.push_back(object);

	mIndexTable[object->GetUniqueID()] = index;

}

void Quad::ComponentUpdateSystem::RemoveObject(Object* object)
{
	size_t index = mIndexTable[object->GetUniqueID()];

	Object* lastObject = mObjectVector[mObjectVector.size() - 1];

	std::swap(mObjectVector[mObjectVector.size() - 1], mObjectVector[index]);
	mObjectVector.pop_back();
	mIndexTable.erase(object->GetUniqueID());


	mIndexTable[lastObject->GetUniqueID()] = index;


}

void Quad::ComponentUpdateSystem::Reset()
{
	mObjectVector.clear();
	mIndexTable.clear();


}

std::vector<Quad::Object*>& Quad::ComponentUpdateSystem::GetObjectVector()
{
	return mObjectVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

const std::vector<Quad::Object*>& Quad::ComponentUpdateSystem::GetObjectVector() const
{
	return mObjectVector;
	// TODO: 여기에 return 문을 삽입합니다.
}
