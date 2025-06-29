#include "PanelUiEntity.h"

#include"MouseEvent.h"
#include"Collision/CollisionHelper.h"
#include"Component/UiColliderComponent.h"
#include"Map/Map.h"

#include"Component/UIMeshComponent.h"


Quad::PanelUiEntity::PanelUiEntity(const std::string& name)
	:UiEntity(name)
{
}

Quad::PanelUiEntity::~PanelUiEntity()
{
}

void Quad::PanelUiEntity::Initialize()
{
	UiEntity::Initialize();


}

Quad::PanelUiEntity* Quad::PanelUiEntity::Create(Map* map, int mapLayerIndex)
{
	PanelUiEntity* ob = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating();

	return ob;
}


void Quad::PanelUiEntity::Update(float deltaTime)
{
	UiEntity::Update(deltaTime);
}



void Quad::PanelUiEntity::AddChildPanelUiEntity(UiEntity* childPanel)
{
	mChildPanelUiEntityVector.push_back(childPanel);
	AddChildObject(childPanel);
	
	//디폴값 0이 아니라 특정효과를 위해 설정했다면 자식들도 모두 같은값을 가진다.
	if(GetStencilRefValue()!=0)
		childPanel->SetStencilRefValue(GetStencilRefValue());
}

void Quad::PanelUiEntity::RemoveChildObject(Object* childObject)
{
	UiEntity::RemoveChildObject(childObject);

	
	auto it =	std::find(mChildPanelUiEntityVector.begin(), mChildPanelUiEntityVector.end(), childObject);
	if (it != mChildPanelUiEntityVector.end())
		mChildPanelUiEntityVector.erase(it);

}

bool Quad::PanelUiEntity::RemoveChildObjectInVector(Object* childObject)
{
	bool ret =UiEntity::RemoveChildObjectInVector(childObject);

	if (ret == true)
	{
		auto it = std::find(mChildPanelUiEntityVector.begin(), mChildPanelUiEntityVector.end(), childObject);
		if (it != mChildPanelUiEntityVector.end())
			mChildPanelUiEntityVector.erase(it);
	}
	return ret;
}

const std::vector<Quad::UiEntity*>& Quad::PanelUiEntity::GetChildPanelUiEntityVector() const
{

	return mChildPanelUiEntityVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

std::vector<Quad::UiEntity*>& Quad::PanelUiEntity::GetChildPanelUiEntityVector()
{
	return mChildPanelUiEntityVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::PanelUiEntity::SetDrawFlag(bool flag)
{
	UiEntity::SetDrawFlag(flag);

	const std::vector<UiEntity*>& childPanelVector = GetChildPanelUiEntityVector();

	std::for_each(childPanelVector.begin(), childPanelVector.end(), [flag](UiEntity* childPanel){
		childPanel->SetDrawFlag(flag);
		});


}

Quad::Object* Quad::PanelUiEntity::GetChildHoverObject() const
{
	return mHoverChildObject;


}

void Quad::PanelUiEntity::SetChildHoverObject(Object* childObject)
{
	mHoverChildObject = childObject;


}


void Quad::PanelUiEntity::HandleLButtonDown(const MouseInputData& mouseInputData)
{


}

void Quad::PanelUiEntity::HandleLButtonUp(const MouseInputData& mouseInputData)
{

}

void Quad::PanelUiEntity::HandleRButtonDown(const MouseInputData& mouseInputData)
{


}

void Quad::PanelUiEntity::HandleRButtonUp(const MouseInputData& mouseInputData)
{


}

void Quad::PanelUiEntity::HandleMouseMove(const MouseInputData& mouseInputData)
{
	

}

void Quad::PanelUiEntity::HandleHover(const MouseInputData& mouseInputData)
{
	

}

void Quad::PanelUiEntity::InitCreating()
{
	UiEntity::InitCreating();

	UIMeshComponent * uiMeshComponent = 	GetUiMeshComponent();
	uiMeshComponent->SetTexture("Default.bmp");
	uiMeshComponent->SetEffect("DefaultUi.effect");


}


