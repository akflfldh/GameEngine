#include "EditorDirector/DefaultState.h"
#include"Core/MouseClickState.h"
#include"Core/MouseClickSelectEvent.h"
#include "Core/MouseEvent.h"
#include"System/GamePlayUiSystem.h"
#include"Event/EventTable.h"
#include"Core/KeyBoard.h"
//#include"Text.h"
#include"EditorDirector/Window.h"
#include"Core/MouseInOutEvent.h"
#include"Collision/CollisionWorld.h"
#include"Core/SelectEvent.h"
#include"Core/Mouse.h"
#include"System/AttributeUiSystem.h"
#include"EditorDirector/GameObjectSelectEvent.h"

#include"Utility/MathHelper.h"

#include"Core/System.h"


#include"Object/Camera/CameraHelper.h"

namespace Quad
{

	DefaultState::DefaultState(const std::string& name)
		:SystemState(name)
	{
	}

	void DefaultState::EnterState(System& system)
	{
	}

	void DefaultState::RunState(System& system)
	{
	}

	void DefaultState::ExitState(System& system)
	{
	}

	SystemState* DefaultState::HandleEvent(System& system, Event* event)
	{

		const std::string& eventName = event->GetName();





		if (eventName == "DragAndDrop")
		{
			//int a = 2;
		}
		else if (eventName == "LButtonDownDouble")
		{
			HandleLButtonDownDouble(system, event);
		}
		else if (eventName == "LButtonDown")
		{
			return HandleLButtonDownEvent(system, event);
		}
		else if (eventName == "LButtonUp")
		{
			HandleLButtonUpEvent(system, event);
		}
		else if (eventName == "RButtonDown")
		{
			HandleRButtonDownEvent(system, event);
		}
		else if (eventName == "RButtonUp")
		{
			HandleRButtonUpEvent(system, event);
		}
		else if (eventName == "MouseMove")
		{
			HandleMouseMoveEvent(system, event);

		}
		else if (eventName == "KeyDown")
		{

			HandleKeyDownEvent(system, event);

		}
		else if (eventName == "KeyUp")
		{

			HandleKeyDownEvent(system, event);
		}
		else if (eventName == "MouseInOut")
		{
			HandleMouseInOutEvent(system, event);
		}
		else if (eventName == "GamePlay")
		{






		}
		/*else if (eventName == "GameObjectSelect")
		{
			HandleGameObjectSelectEvent(system, event);
			


		}*/
		else
		{
			//switch (event->GetEventID())
			//{
			//case EEventID::eSelectEvent:

			//{
			//	SelectEvent* selectEvent = (SelectEvent*)event;
			//	Object * object= selectEvent->GetSelectedObject();

			//	
			//	//다른창들에게 적절한 화면을 출력하도록하고
			//	
			//	//시스템에게 선택되었다고 이벤트 를 계속연쇄적으로 전달
			//	//시스템은 기즈모를 그 엔티티중앙에띄운다. 
			//	//


			//}
			//	break;
			
			DefaultHandleEvents(system, event);


		}



		return nullptr;
	}

	void DefaultState::Reset()
	{
		mHoverObject = nullptr;

	}



	SystemState* DefaultState::HandleLButtonDownEvent(System& system, Event* event)
	{

		Object* capturedObject = Mouse::GetCapturedObject();
		if (capturedObject != nullptr && capturedObject->GetSystem()->GetSystemID() != system.GetSystemID())
		{
			return nullptr;
		}
		
		if (event->GetSystemID() != system.GetSystemID() &&  event->GetSystemID() != ESystemID::eNone)
		{
			return nullptr;
		}




		MouseEvent* mouseEvent = (MouseEvent*)event;

		UINT screenPosX = mouseEvent->GetClientPositionX();
		UINT screenPosY = mouseEvent->GetClientPositionY();

		Quad::Map* map = system.GetMap();
		//CollisionWorld * collisionWorld = map->GetCollisionWorld();
		
		
		
		Object* SelectedObject= nullptr;

		//반직선 계산이 필요하고
		/*Ray*/
		//또는 그반직선을 여기서 계산할지 아니면 Controller가 계산해서 줄지 

		DirectX::XMFLOAT2 screenPos = { (float)mouseEvent->GetClientPositionX(),(float)mouseEvent->GetClientPositionY() };
		Camera* cameraUi = map->GetMainCamera(0);
		Camera* camera3D = map->GetMainCamera(1);


		Ray rayUi;
		Ray ray3D;


		if (cameraUi)
		{
			CameraHelper::CalculateRay(screenPos, system.GetViewPort(), *cameraUi, true, rayUi);
		}

		if (camera3D)
		{
			CameraHelper::CalculateRay(screenPos, system.GetViewPort(), *camera3D, true, ray3D);
		}

		MouseEvent mouseEventUi(*mouseEvent);
		MouseEvent mouseEvent3D(*mouseEvent);


		mouseEventUi.SetRay(rayUi);
		mouseEvent3D.SetRay(ray3D);

		mouseEventUi.SetGlobalFlag(false);
		mouseEvent3D.SetGlobalFlag(false);
		//mouse lbuttondown event를 등록된 엔티티들에게 모두 전달한다.
		//혹은 오브젝트들이 콜백함수를 등록하게할수도있는데

		std::vector<Object*> objectVector;
		const EventTable * eventTable = system.GetEventTable();
		
		//레이와 충돌하지않고도 항상 LButtonDown 이벤트를 받을려고 등록된 오브젝트들
		eventTable->GetObjectVector("LButtonDown", objectVector);
	//	mouseEvent->SetRay(ray);
	//	mouseEvent->SetGlobalFlag(false);
		//
		//부모로전달된 EVENT를 자식에게 전달할것이냐말것이냐는, 각 엔티티마다 결정할일이다.
		//시스템은 항상 등록된 엔티티들에게만전달한다.
		
		




		if (capturedObject == nullptr)
		{
			for (int i = 0l; i < objectVector.size(); ++i)
			{
				if (cameraUi && objectVector[i]->GetObjectType() == EObjectType::eUiEntity)
				{
					objectVector[i]->OnEvent(&mouseEventUi);
				}
				else if(camera3D)
				{
					objectVector[i]->OnEvent(&mouseEvent3D);
				}

			}
		}
		else
		{
			if(capturedObject->GetObjectType() == EObjectType::eUiEntity)
				capturedObject->OnEvent(&mouseEventUi);
			else
				capturedObject->OnEvent(&mouseEvent3D);

		}

		//기즈모를 먼저 판단해야한다.
		//그럼맵에 충돌월드에서도 기즈모를 유지하고있어야하나?
		//단 기즈모가 active되었다,select flag가 켜졌다는가정하에
			

		//if (collisionWorld->RayCastingFirst(SelectedObject, ray))
		//if (map->RayCastingFirst(screenPos, SelectedObject))
		//{
		//	
		//	
		//	//if (!SelectedObject->GetSelectAvailableFlag())
		//	//	return nullptr;


		//	if (!system.GetRuntimeModeState())
		//	{//edit모드이다.

		//		//그러면 기즈모를 띄운다.
		//		//그 오브젝트위치에
		//	//	const Gizmo *  gizmo = system.GetGizmo();


		//		//Gizmo* gizmo = system.GetMap()->GetGizmo();
		//		//if (!(SelectedObject == gizmo || gizmo->CheckChildHandleSelect(SelectedObject)))
		//		//{
		//		//	//system.OnGizmo(SelectedObject);

		//		//	gizmo->SetDestObject(SelectedObject);

		//		//	gizmo->OnGizmo();

		//		//}
		//	}

	
		//	mouseEvent->SetGlobalFlag(false);

		//	if (capturedObject == nullptr || capturedObject == SelectedObject)
		//	{
		//		//SelectedObject->OnEvent(mouseEvent);

		//		//SetEventFocusFlag(true);

		//		//text이것도 이제는 텍스트 내부에서 처리할수있겠다.
		//		/*if (dynamic_cast<Text*>(SelectedObject))
		//			KeyBoard::SetTextBoxActiveFlag(true);
		//		else
		//			KeyBoard::SetTextBoxActiveFlag(false);*/

		//	}



		//	return nullptr;

		//}
		//else
		//{
		//	//충돌하는게 없다.
		//	if (!system.GetRuntimeModeState())
		//	{//edit모드이다.

		//		////그러면 기즈모를 띄운다.
		//		////그 오브젝트위치에
		//		//Gizmo* gizmo = system.GetMap()->GetGizmo();
		//		//gizmo->SetDestObject(nullptr);

		//		//gizmo->OffGizmo();

		//	}

		//}

		return nullptr;


	}

	SystemState* DefaultState::HandleKeyDownEvent(System& system, Event* event)
	{
		std::vector<Object*> keyDownObjectVector;

		const EventTable* eventTable = system.GetEventTable();
		eventTable->GetObjectVector(event->GetName(), keyDownObjectVector);
	

		for (int i = 0; i < keyDownObjectVector.size(); ++i)
		{
			if (keyDownObjectVector[i]->GetActiveFlag())
			{
				keyDownObjectVector[i]->OnEvent(event);
			}
		}


		//선택된놈들, 그리고 선택되지않아도 전역적으로받을려는애들한테 보내야겠다.


		return nullptr;

	}

	SystemState* DefaultState::HandleKeyUpEvent(System& system, Event* event)
	{
		std::vector<Object*> keyUpObjectVector;
		system.GetEventTable()->GetObjectVector(event->GetName(), keyUpObjectVector);

		for (int i = 0; i < keyUpObjectVector.size(); ++i)
		{
			keyUpObjectVector[i]->OnEvent(event);
		}

		return nullptr;
	}
	SystemState* DefaultState::HandleMouseMoveEvent(System& system, Event* pEvent)
	{

		Object* capturedObject = Mouse::GetCapturedObject();
		if (capturedObject != nullptr && capturedObject->GetSystem()->GetSystemID() != system.GetSystemID())
		{
			return nullptr;
		}

		if (pEvent->GetSystemID() != system.GetSystemID() && pEvent->GetSystemID() != ESystemID::eNone)
		{
			return nullptr;
		}


		MouseEvent* mouseEvent = (MouseEvent*)pEvent;
		DirectX::XMFLOAT2 clientPos = { (float)mouseEvent->GetClientPositionX(), (float)mouseEvent->GetClientPositionY() };

		//hover 이벤트를 받는 엔티티들에대해서만 ray 충돌판정을 수행하고
		//충돌한다면 그 엔티티에 hover이벤트를 보낸다.
		
		//hover리스트에서만 하면안되는이유, 만약 hover처리를 안받는 물체가 앞에서 가리고있는데도 리스트에 등록된 물체만감지해서 
		//hover이벤트를 전송하게됨으로 이렇게해서는안된다.이거는 시각적인 장면의 가시성과 관련된것임으로


		Map* map = system.GetMap();
		Camera * cameraUi = map->GetMainCamera(0);
		Camera * camera3D = map->GetMainCamera(1);

		Ray rayUi;
		Ray ray3D;

		MouseEvent mouseEventUi(*mouseEvent);
		MouseEvent mouseEvent3D(*mouseEvent);

		if (cameraUi)
		{
			CameraHelper::CalculateRay(clientPos, system.GetViewPort(), *cameraUi, true, rayUi);
			mouseEventUi.SetRay(rayUi);
		}

		if (camera3D)
		{
			CameraHelper::CalculateRay(clientPos, system.GetViewPort(), *camera3D, true, ray3D);
			mouseEvent3D.SetRay(ray3D);
		}

		//Camera* camera = system.GetMap()->GetMainCamera();
	//	Ray ray;
		//CameraHelper::CalculateRay(clientPos, system.GetViewPort(), *camera, true, ray);
		//mouseEvent->SetRay(ray);
		//MouseMove EVENT



		if (capturedObject == nullptr)
		{

			std::vector<Object*> objectVector;
			system.GetEventTable()->GetObjectVector("MouseMove", objectVector);

			for (int i = 0; i < objectVector.size(); ++i)
			{
				if (cameraUi && objectVector[i]->GetObjectType() == EObjectType::eUiEntity)
				{
					objectVector[i]->OnEvent(&mouseEventUi);
				}
				else if (camera3D)
				{
					objectVector[i]->OnEvent(&mouseEvent3D);
				}
				
			}
		}
		else
		{
			if (capturedObject->GetObjectType() == EObjectType::eUiEntity || capturedObject->GetObjectType() ==EObjectType::eSpline)
			{
				capturedObject->OnEvent(&mouseEventUi);
			}
			else
			{
				capturedObject->OnEvent(&mouseEvent3D);
			}
		}





		//Hover
		
		
		MouseEvent hoverEventUi("Hover");
		MouseEvent hoverEvent3D("Hover");

		hoverEventUi.SetClientPosition(mouseEvent->GetClientPositionX(), mouseEvent->GetClientPositionY());
		hoverEventUi.SetScreenPosition(mouseEvent->GetScreenPositionX(), mouseEvent->GetScreenPositionY());
		hoverEventUi.SetRay(rayUi);

		hoverEvent3D.SetClientPosition(mouseEvent->GetClientPositionX(), mouseEvent->GetClientPositionY());
		hoverEvent3D.SetScreenPosition(mouseEvent->GetScreenPositionX(), mouseEvent->GetScreenPositionY());
		hoverEvent3D.SetRay(ray3D);

		//자식창으로 , 다른창으로 마우스가움직이면 더이상못받으니 문제가생겼다.
		//다른창으로 바로마우스가 가도 hover를 풀어야한다.
		//창이 벗어난거를 생각해야돼 
		//

		Object* object=nullptr; 
		//if (system.GetMap()->GetCollisionWorld()->RayCastingFirst(object, ray))
		if (map->RayCastingFirst(clientPos, object))
		{
			
			if (mHoverObject == object)
			{
				//hover는 딱 한번만 보낸다.
				if (mHoverObject->GetStateComponent()->GetHoverState() == false)
				{
					//독점해서 가질경우  해제되어도 hoverObject가 유지되고 그상테에서 다시 움직일떄 처리해줘야하니

						
					if (capturedObject == nullptr || capturedObject == object)
					{
						if (object->GetObjectType() == EObjectType::eUiEntity)
						{
							hoverEventUi.SetHoverState(true);
							object->OnEvent(&hoverEventUi);
						}
						else
						{
							hoverEvent3D.SetHoverState(true);
							object->OnEvent(&hoverEvent3D);
						}

					}
				}
				
			}else  if (mHoverObject == nullptr)
			{
			
				if (capturedObject == nullptr || capturedObject == object)
				{
					if (object->GetObjectType() == EObjectType::eUiEntity)
					{
						hoverEventUi.SetHoverState(true);
						object->OnEvent(&hoverEventUi);
					}
					else
					{
						hoverEvent3D.SetHoverState(true);
						object->OnEvent(&hoverEvent3D);
					}

					mHoverObject = object;
				}
			
			}else if (mHoverObject != object)
			{

					//순서는 hover false먼저,	
				hoverEventUi.SetHoverState(false);
				hoverEvent3D.SetHoverState(false);


				if (capturedObject == nullptr || capturedObject == mHoverObject)
				{
					if (mHoverObject->GetObjectType() == EObjectType::eUiEntity)
					{
						mHoverObject->OnEvent(&hoverEventUi);
					}
					else
					{
						mHoverObject->OnEvent(&hoverEvent3D);
					}



					if (mHoverObject->GetStateComponent()->GetExclusiveHoverEventFlag())
					{

						//hoverEvent를 독점하고있다.
						return nullptr;
					}
				}

			
				hoverEventUi.SetHoverState(true);
				hoverEvent3D.SetHoverState(true);


				if (capturedObject == nullptr || capturedObject == object)
				{
					if (object->GetObjectType() == EObjectType::eUiEntity)
					{
						object->OnEvent(&hoverEventUi);
					}
					else
					{
						object->OnEvent(&hoverEvent3D);
					}
					//object->OnEvent(&hoverEvent);
					mHoverObject = object;
				}

			}


		}
		else
		{
			if (mHoverObject != nullptr)
			{
				hoverEventUi.SetHoverState(false);
				hoverEvent3D.SetHoverState(false);

				if (capturedObject == nullptr || capturedObject == mHoverObject)
				{
					int a = 2;
					if (mHoverObject->GetObjectType() == EObjectType::eUiEntity)
					{

						mHoverObject->OnEvent(&hoverEventUi);
					}
					else
					{
						mHoverObject->OnEvent(&hoverEvent3D);

					}

					if (mHoverObject->GetStateComponent()->GetExclusiveHoverEventFlag())
					{
						//hoverEvent를 독점하고있다. 계속받는다.
						return nullptr;
					}

					mHoverObject = nullptr;
				}


			}
		}


		return nullptr;

	}


	SystemState* DefaultState::HandleMouseInOutEvent(System& system, Event* event)
	{
		MouseInOutEvent* mouseInOutEvent = (MouseInOutEvent*)event;

		switch (mouseInOutEvent->GetMouseState())
		{
		case EMouseInOut::eOut:
		{
			if (mHoverObject != nullptr)
			{
				MouseEvent hoverEvent("Hover");
				hoverEvent.SetHoverState(false);
				mHoverObject->OnEvent(&hoverEvent);
				mHoverObject = nullptr;
			}
		}
		}

		return nullptr;
	}
	SystemState* DefaultState::HandleLButtonUpEvent(System& system, Event* event)
	{

		Object* capturedObject = Mouse::GetCapturedObject();

		//LButtonUpEvent를 받을려는 엔티티들에게 전송
		if (capturedObject != nullptr && capturedObject->GetSystem()->GetSystemID() != system.GetSystemID())
		{
			return nullptr;
		}

		if (event->GetSystemID() != system.GetSystemID() && event->GetSystemID() != ESystemID::eNone)
		{
			return nullptr;
		}


		const EventTable* eventTable = system.GetEventTable();
		std::vector<Object*> objectVector ;
		eventTable->GetObjectVector("LButtonUp", objectVector);

		if (capturedObject == nullptr)
		{
			for (int i = 0; i < objectVector.size(); ++i)
				objectVector[i]->OnEvent(event);
		}
		else
		{
			capturedObject->OnEvent(event);
		}



	/*	Object* selectObject = system.GetSelectObject();
		if(selectObject != nullptr)
		{
			selectObject->OnEvent(event);
		}
		*/

		return nullptr;


	}
	void DefaultState::HandleRButtonDownEvent(System& system, Event* event)
	{
		Object* capturedObject = Mouse::GetCapturedObject();

		if (capturedObject != nullptr && capturedObject->GetSystem()->GetSystemID() != system.GetSystemID())
		{
			return ;
		}

		if (event->GetSystemID() != system.GetSystemID() && event->GetSystemID() != ESystemID::eNone)
		{
			return ;
		}


		const EventTable* eventTable = system.GetEventTable();
		std::vector<Object*> objectVector;
		eventTable->GetObjectVector("RButtonDown", objectVector);


		if (capturedObject == nullptr)
		{
			for (int i = 0; i < objectVector.size(); ++i)
				objectVector[i]->OnEvent(event);
		}
		else
		{
			capturedObject->OnEvent(event);
		}
		
		//Object* selectObject = system.GetSelectObject();
		//if (selectObject != nullptr)
		//{
		//	selectObject->OnEvent(event);
		//}


		return;


	}
	void DefaultState::HandleRButtonUpEvent(System& system, Event* event)
	{
		Object* capturedObject = Mouse::GetCapturedObject();
		if (capturedObject != nullptr && capturedObject->GetSystem()->GetSystemID() != system.GetSystemID())
		{
			return;
		}

		if (event->GetSystemID() != system.GetSystemID() && event->GetSystemID() != ESystemID::eNone)
		{
			return;
		}


		const EventTable* eventTable = system.GetEventTable();
		std::vector<Object*> objectVector;
		eventTable->GetObjectVector("RButtonUp", objectVector);


		if (capturedObject == nullptr)
		{
			for (int i = 0; i < objectVector.size(); ++i)
				objectVector[i]->OnEvent(event);
		}
		else
		{
			capturedObject->OnEvent(event);

		}
		return;
	}

	void DefaultState::HandleLButtonDownDouble(System& system, Event* event)
	{
		Object* capturedObject = Mouse::GetCapturedObject();
		if (capturedObject != nullptr && capturedObject->GetSystem()->GetSystemID() != system.GetSystemID())
		{
			return;
		}

		if (event->GetSystemID() != system.GetSystemID() && event->GetSystemID() != ESystemID::eNone)
		{
			return;
		}

		const EventTable* eventTable = system.GetEventTable();
	

		if (capturedObject == nullptr)
		{
			std::vector<Object*> objectVector;
			eventTable->GetObjectVector(event->GetName(), objectVector);

			for (int i = 0; i < objectVector.size(); ++i)
				objectVector[i]->OnEvent(event);
		}
		else
		{
			capturedObject->OnEvent(event);

		}


	}

	void DefaultState::HandleGameObjectSelectEvent(System& system, Event* event)
	{
		//syste state구조를 바꿔야한다.

		//attribute ui system 이다 

	/*	GameObjectSelectEvent* selectEvent = (GameObjectSelectEvent*)event;

		AttributeUiSystem& attrUiSystem = (AttributeUiSystem&)system;

		attrUiSystem.SetSelectedObject(selectEvent->GetSelectObject());



		const EventTable* eventTable = system.GetEventTable();

		eventTable->GetObjectVector(event->GetName())
*/



		//이것도 그냥 그 시스템에 엔티티들에게 이벤트를전달하는게좋을거같은데
		//그러면 각자알아서 선택된오브젝트에대해서 작업을 수행하겠지.





	}



	void DefaultState::DefaultHandleEvents(System& system, Event* event)
	{

		const EventTable* eventTable = system.GetEventTable();
		std::vector<Object*> objectVector;
		eventTable->GetObjectVector(event->GetName(), objectVector);

		for (int i = 0; i < objectVector.size(); ++i)
			objectVector[i]->OnEvent(event);

		return;
	}
	//디폴트 상태에서는 키입력을 안받는게 맞는거같다.
	//누군가 선택하고 key입력이 그 엔티티에게 들어가는게 맞는거지
	//혹여나 텍스일수있으니




}