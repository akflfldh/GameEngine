#include "PanelUiEntity.h"

#include"MouseEvent.h"
#include"Collision/CollisionHelper.h"
#include"Component/UiColliderComponent.h"
#include"Map/Map.h"


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

void Quad::PanelUiEntity::OnEvent(Event* event)
{
//	UiEntity::OnEvent(event);


	const std::string& eventName = event->GetName();
	if (eventName == "LButtonDown")
	{
		HandleLButtonDown(event);
	}
	else if (eventName == "LButtonUp")
	{
		HandleLButtonUp(event);

	}
	else if (eventName == "RButtonDown")
	{
		HandleRButtonDown(event);

	}
	else if (eventName == "RButtonUp")
	{
		HandleRButtonUp(event);

	}
	else if (eventName == "MouseMove")
	{
		
		HandleMouseMove(event);
	}
	else if (eventName == "Hover")
	{

		HandleHover(event);

	}
	else
	{
		if (eventName == "KeyDown")
		{
			int a = 2;
		}
		HandleDefaultEvent(event);
	}
	














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

void Quad::PanelUiEntity::HandleLButtonDown(Event* pEvent)
{




	StateComponent* stateComponent = GetStateComponent();

	//auto callback = GetEventCallback("LButtonDown");
	//if (callback != nullptr)
	//	(*callback)(pEvent);

	CallEventCallback("LButtonDown", pEvent);



	if (stateComponent->GetHoverState())
	{
		stateComponent->SetLButtonDownState(true);
	}

	
	for (int i = 0; i < mChildPanelUiEntityVector.size(); ++i)
		mChildPanelUiEntityVector[i]->OnEvent(pEvent);



	



	//MouseEvent* mouseEvent = (MouseEvent*)pEvent;

	//const Ray& ray = mouseEvent->GetRay();

	//float minT = FLT_MAX;
	//int index = -1;
	//for (int i = 0; i < mChildPanelUiEntityVector.size(); ++i)
	//{
	//	float t = FLT_MAX;
	//	if (CollisionHelper::Intersect(mChildPanelUiEntityVector[i]->GetCollider(), ray, t))
	//	{
	//		if (minT > t)
	//		{
	//			minT = t;
	//			index = i;
	//		}

	//	}

	//}

	//if (index != -1)
	//{
	//	mChildPanelUiEntityVector[index]->OnEvent(pEvent);
	//}





}

void Quad::PanelUiEntity::HandleLButtonUp(Event* pEvent)
{


	StateComponent* stateComponent = GetStateComponent();


	//auto callback = GetEventCallback("LButtonUp");
	//if (callback != nullptr)
	//	(*callback)(pEvent);

	CallEventCallback("LButtonUp", pEvent);


	if (stateComponent->GetLButtonDownState())
	{
		stateComponent->SetLButtonDownState(false);


		for (int i = 0; i < mChildPanelUiEntityVector.size(); ++i)
		{
			mChildPanelUiEntityVector[i]->OnEvent(pEvent);
		}

	}



}

void Quad::PanelUiEntity::HandleRButtonDown(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();

	//auto callback = GetEventCallback("RButtonDown");
	//if (callback != nullptr)
	//	(*callback)(pEvent);

	CallEventCallback("RButtonDown", pEvent);



	if (stateComponent->GetHoverState())
	{
		stateComponent->SetLButtonDownState(true);
	}


	for (int i = 0; i < mChildPanelUiEntityVector.size(); ++i)
		mChildPanelUiEntityVector[i]->OnEvent(pEvent);










}

void Quad::PanelUiEntity::HandleRButtonUp(Event* pEvent)
{


	StateComponent* stateComponent = GetStateComponent();

	//auto callback = GetEventCallback("RButtonUp");
	//if (callback != nullptr)
	//	(*callback)(pEvent);

	CallEventCallback("RButtonUp", pEvent);



	if (stateComponent->GetHoverState())
	{
		stateComponent->SetLButtonDownState(true);
	}


	for (int i = 0; i < mChildPanelUiEntityVector.size(); ++i)
		mChildPanelUiEntityVector[i]->OnEvent(pEvent);



}

void Quad::PanelUiEntity::HandleMouseMove(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;

	//auto callback = GetEventCallback("MouseMove");
	//if (callback != nullptr)
	//	(*callback)(pEvent);

	CallEventCallback("MouseMove", pEvent);



	if (stateComponent->GetHoverState())
	{

		//자식들에대해서 ray판정을 수행

		//이미 hover인지아닌지도 판단해서 hover상태 전송
		const Ray& ray = mouseEvent->GetRay();

		float minT = FLT_MAX;
		int index = -1;
		for (int i = 0; i < mChildPanelUiEntityVector.size(); ++i)
		{
			float t = FLT_MAX;
			//보이지않은 엔티티들이 피킹되는것은 이상하다.
			if (mChildPanelUiEntityVector[i]->GetDrawFlag())
			{

				if (CollisionHelper::Intersect(mChildPanelUiEntityVector[i]->GetModel()->GetUiColliderComponent()->GetCollider(), ray, t))
				{
					if (minT > t)
					{
						minT = t;
						index = i;
					}

				}
			}

		}

		
		if (index != -1)
		{
			if (mChildPanelUiEntityVector[index]->GetStateComponent()->GetHoverState() == false )
			{
				//hover event 
				MouseEvent hover("Hover");
				hover.SetHoverState(true);
				mChildPanelUiEntityVector[index]->OnEvent(&hover);

				if (mHoverChildObject != nullptr && mChildPanelUiEntityVector[index] != mHoverChildObject)
				{
					hover.SetHoverState(false);
					mHoverChildObject->OnEvent(&hover);
				}
				mHoverChildObject = mChildPanelUiEntityVector[index];
			}
			else
			{

			}

		}
		else
		{

			if (mHoverChildObject != nullptr)
			{
				MouseEvent hover("Hover");
				hover.SetHoverState(false);
				mHoverChildObject->OnEvent(&hover);
				mHoverChildObject = nullptr;
			}
		}

		//mouse move

		for (int i = 0; i < mChildPanelUiEntityVector.size(); ++i)
		{
			mChildPanelUiEntityVector[i]->OnEvent(pEvent);
	
		}


	}








}

void Quad::PanelUiEntity::HandleHover(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;
	stateComponent->SetHoverState(mouseEvent->GetHoverState());
	

	//auto callback = GetEventCallback("Hover");
	//if (callback !=nullptr)
	//	(*callback)(pEvent);

	CallEventCallback("Hover", pEvent);


	if (mouseEvent->GetHoverState())
	{



	}
	else 
	{

		//마우스가 빠르게움직여서 자식들이 mouseMove메시지를 못받아서 정확히 hover처리를못할수도있음으로
		//보낸다.
	
		for (int i = 0; i < mChildPanelUiEntityVector.size(); ++i)
		{
			mChildPanelUiEntityVector[i]->OnEvent(pEvent);
		}
		mHoverChildObject = nullptr;
		
	}


}

void Quad::PanelUiEntity::InitCreating()
{
	UiEntity::InitCreating();
	SetTexture("Default.bmp");
	//SetSize(1000, 1000);
	SetEffect("DefaultUi.effect");
	//SetPosition(0, 0, 10.0f);











}

//void Quad::PanelUiEntity::HandleDefault(Event* pEvent)
//{
//
//	auto eventCallback = GetEventCallback(pEvent->GetName());
//	
//	if(eventCallback!=nullptr)
//		eventCallback(pEvent);
//
//
//}

