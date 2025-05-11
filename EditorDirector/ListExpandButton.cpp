#include "ListExpandButton.h"
#include"System.h"

#include"MouseEvent.h"
#include"MathHelper.h"
#include"Utility.h"
Quad::ListExpandButton::ListExpandButton(const std::string& name)
	:ButtonUiEntity(name),mExpandFlag(false)
{
}

void Quad::ListExpandButton::Initialize()
{
	
	ButtonUiEntity::Initialize();
	GetSystem()->RegisterEntityToEvent("LButtonDown", this);
	GetSystem()->RegisterEntityToEvent("LButtonUp", this);
	GetSystem()->RegisterEntityToEvent("MouseMove", this);
	GetSystem()->RegisterEntityToEvent("Hover", this);

	







}

Quad::ListExpandButton* Quad::ListExpandButton::Create(Map* map, int mapLayerIndex)
{
	ListExpandButton* ob = static_cast<ListExpandButton*>(map->CreateObject("ListExpandButton", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating();

	return ob;
}


void Quad::ListExpandButton::Update(float deltaTime)
{







}

void Quad::ListExpandButton::OnEvent(Event* event)
{
	const std::string& eventName = event->GetName();

	if (eventName == "Hover")
	{
		HandleHover(event);
	}
	else if (eventName == "LButtonDown")
	{
		HandleLButtonDown(event);
	}
	else if (eventName == "LButtonUp")
	{
		HandleLButtonUp(event);
	}
	else if (eventName == "MouseMove")
	{
		HandleMouseMove(event);
	}




}

int Quad::ListExpandButton::GetExpandFlag() const
{
	return mExpandFlag;
}

void Quad::ListExpandButton::LogicalButtonClick()
{


	mExpandFlag ^= 1;
	if (mExpandFlag)
	{//펼쳐짐

		GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetTextureTransformMatrix(0, mExpandTransformTexMatrixF);

	}
	else
	{//닫힘

		GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetTextureTransformMatrix(0, Utility::GetIdentityMatrixF());

	}

	auto callback = GetLButtonUpCallback();
	if(callback!=nullptr)
		(*callback)();

}

void Quad::ListExpandButton::InitCreating()
{
	ButtonUiEntity::InitCreating();


	SetTexture("ExpandButton.png");
	GetTransform().SetIndependentScaleFlag(true);
	SetEffect("DefaultUi.effect");



	DirectX::XMMATRIX preRotatePivotMatrix = DirectX::XMMatrixTranslation(-0.5f, -0.5f, 0);
	DirectX::XMMATRIX rotateMatrix = DirectX::XMMatrixRotationRollPitchYaw(0, 0, -90.0f);
	DirectX::XMMATRIX postRotatePivotMatrix = DirectX::XMMatrixTranslation(0.5f, 0.5f, 0);

	DirectX::XMStoreFloat4x4(&mExpandTransformTexMatrixF, DirectX::XMMatrixTransformation2D({ 0,0,0 }, 1, { 1,1,1 }, { 0.5f,0.5f,0 }, -90.0f * DirectX::XM_PI / 180.0F, { 0,0,0 }));


}



void Quad::ListExpandButton::HandleHover(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;



	stateComponent->SetHoverState(mouseEvent->GetHoverState());

	if (stateComponent->GetLButtonDownState())
		return;


	if (mouseEvent->GetHoverState())
	{
		SetColorItensity({ 2.0f,2.0f,2.0f });
		auto callback = GetHoverCallback();
		if (callback != nullptr)
			(*callback)();

	}
	else
	{
		
		SetColorItensity({ 1.0f,1.0f,1.0f });
		auto  callback = GetReleaseHoverCallback();
		if (callback != nullptr)
			(*callback)();
	}
	

}

void Quad::ListExpandButton::HandleMouseMove(Event* pEvent)
{



}

void Quad::ListExpandButton::HandleLButtonDown(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetHoverState())
	{
		stateComponent->SetLButtonDownState(true);
	}


}

void Quad::ListExpandButton::HandleLButtonUp(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetLButtonDownState())
	{
		stateComponent->SetLButtonDownState(false);



		//if(mLButtonUpCallback!=nullptr)
			//mLButtonUpCallback();

	







		LogicalButtonClick();//동일하다
		//mExpandFlag ^= 1;
		//if (mExpandFlag)
		//{//펼쳐짐

		//	GetModel()->GetSubMesh()[0].mMateiral->SetTextureTransformMatrix(0, mExpandTransformTexMatrixF);

		//}
		//else
		//{//닫힘

		//	GetModel()->GetSubMesh()[0].mMateiral->SetTextureTransformMatrix(0, Utility::GetIdentityMatrixF());

		//}



		//auto callback = GetLButtonUpCallback();
		//callback();
	


	}


}
