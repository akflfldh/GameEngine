#include "AssetImageFrameEntity.h"
#include"GameObjectSelectEvent.h"
#include"Map/Map.h"
Quad::AssetImageFrameEntity::AssetImageFrameEntity(const std::string& name)
	:UiEntity(name), mSubMeshIndex(0)
{

}

Quad::AssetImageFrameEntity::~AssetImageFrameEntity()
{
}

void Quad::AssetImageFrameEntity::Initialize()
{
	UiEntity::Initialize();

}

Quad::AssetImageFrameEntity* Quad::AssetImageFrameEntity::Create(Map* map, int mapLayerIndex)
{
	AssetImageFrameEntity* ob = static_cast<AssetImageFrameEntity*>(map->CreateObject("AssetImageFrameEntity", mapLayerIndex));
	ob->Initialize();
	ob->InitCreating();

	return ob;
}

void Quad::AssetImageFrameEntity::Update(float deltaTime)
{
	UiEntity::Update(deltaTime);

}

void Quad::AssetImageFrameEntity::OnEvent(Event* pEvent)
{
	UiEntity::OnEvent(pEvent);

	//const std::string& eventName = pEvent->GetName();

	//if (eventName == "GameObjectSelect")
	//{
	//	HandleGameObjectSelect(pEvent);
	//}



}

void Quad::AssetImageFrameEntity::SetSelectedObject(Object* object)
{
	mSelectedObject = object;
}

Quad::Object* Quad::AssetImageFrameEntity::GetSelectedObject() const
{
	return mSelectedObject;
}

int Quad::AssetImageFrameEntity::GetSubMeshIndex() const
{
	return mSubMeshIndex;
}

void Quad::AssetImageFrameEntity::SetSubMeshIndex(int index)
{
	mSubMeshIndex = index;
}

void Quad::AssetImageFrameEntity::InitCreating()
{
	UiEntity::InitCreating();
	SetEffect("DefaultUi.effect");


}

//void Quad::AssetImageFrameEntity::HandleGameObjectSelect(Event* pEvent)
//{
//
//
//	//GameObjectSelectEvent* selectEvent = (GameObjectSelectEvent*)pEvent;
//	//mSelectedObject = selectEvent->GetSelectObject();
//
//	//
//
//	////원래라면 서브메시 별로 존재해야하지만일단지금은 0번 서브메시에대한 DiffuseMap , 없다면? 설정안함 
//
//
//	////엔티티인경우에해당텍스처존재(asset별로 처리해야함)
//
//
//	//switch (mSelectedObject->GetObjectType())
//	//{
//
//	//case EObjectType::eEntity:
//	//{
//	//	Entity* entity = (Entity*)mSelectedObject;
//	//	Texture * diffuseMap =entity->GetModel()->GetSubMesh()[0].mMateiral->GetDiffuseMap();
//	//	if(diffuseMap  )
//
//
//	//}
//
//
//	//	break;
//	//}
//
//
//
//
//	//SetTexture(mSelectedObject)
//
//
//
//
//
//
//
//
//
//
//
//
//}
