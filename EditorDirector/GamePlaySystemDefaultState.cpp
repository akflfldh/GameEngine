#include "GamePlaySystemDefaultState.h"
#include"NotifyDragAndDropEvent.h"
#include"Core/ContentItem.h"
#include"System.h"
#include"Object/Camera/Camera.h"
#include"MathHelper.h"

#include"Window.h"
#include"Collision/CollisionWorld.h"

#include"Object/Entity.h"
#include"DragAndDropWindowController.h"
#include"MouseEvent.h"
#include"Mouse.h"
#include"Core/EventDispatcher.h"
#include"GameObjectSelectEvent.h"

#include"Component/MeshComponent.h"
#include"Camera/CameraHelper.h"
#include"EditorMap.h"
#include"Gizmo.h"


#include"EditorCollisionWorld.h"


Quad::GamePlaySystemDefaultState::GamePlaySystemDefaultState(const std::string& name)
    :DefaultState(name)
{
    mEntity = new Entity("");

}

Quad::SystemState* Quad::GamePlaySystemDefaultState::HandleEvent(System& system, Event* event)
{

    const std::string& eventName = event->GetName();

    if (eventName == "NotifyDragAndDrop")
    {
        HandleNotifyDragAndDropEvent(system,event);
    }
    else
    {
        //처리하지않은것은 부모DefaultState에게 처리하도록 넘긴다.
     return   DefaultState::HandleEvent(system, event);
    }


    

    return nullptr;


}

Quad::SystemState* Quad::GamePlaySystemDefaultState::HandleLButtonDownEvent(System& system, Event* event)
{
    Object* capturedObject = Mouse::GetCapturedObject();
    if (capturedObject != nullptr && capturedObject->GetSystem()->GetSystemID() != system.GetSystemID())
    {
        return nullptr;
    }

    if (event->GetSystemID() != system.GetSystemID() && event->GetSystemID() != ESystemID::eNone)
    {
        return nullptr;
    }




    MouseEvent* mouseEvent = (MouseEvent*)event;

    UINT screenPosX = mouseEvent->GetClientPositionX();
    UINT screenPosY = mouseEvent->GetClientPositionY();

    Quad::Map* map = system.GetMap();
    //EditorCollisionWorld* collisionWorld =static_cast<EditorCollisionWorld*>( map->GetCollisionWorld());
    Object* SelectedObject;

    //반직선 계산이 필요하고
    /*Ray*/
    //또는 그반직선을 여기서 계산할지 아니면 Controller가 계산해서 줄지 

    DirectX::XMFLOAT2 screenPos = { (float)mouseEvent->GetClientPositionX(),(float)mouseEvent->GetClientPositionY() };
    Camera* cameraUi = map->GetMainCamera(0);
    Camera* camera3D = map->GetMainCamera(1);

    MouseEvent mouseEventUi(*mouseEvent);
    MouseEvent mouseEvent3D(*mouseEvent);

    //Ray ray;
    Ray rayUi;
    Ray ray3D;
    if (cameraUi)
    {
        CameraHelper::CalculateRay(screenPos, system.GetViewPort(), *cameraUi, true, rayUi);
        mouseEventUi.SetRay(rayUi);
    }

    if (camera3D)
    {
        CameraHelper::CalculateRay(screenPos, system.GetViewPort(), *camera3D, true, ray3D);
        mouseEvent3D.SetRay(ray3D);
    }

    //mouse lbuttondown event를 등록된 엔티티들에게 모두 전달한다.
    //혹은 오브젝트들이 콜백함수를 등록하게할수도있는데

    std::vector<Object*> objectVector;
    const EventTable* eventTable = system.GetEventTable();

    //레이와 충돌하지않고도 항상 LButtonDown 이벤트를 받을려고 등록된 오브젝트들
    eventTable->GetObjectVector("LButtonDown", objectVector);



   // mouseEvent->SetRay(ray);
    //
    //부모로전달된 EVENT를 자식에게 전달할것이냐말것이냐는, 각 엔티티마다 결정할일이다.
    //시스템은 항상 등록된 엔티티들에게만전달한다.




    if (capturedObject == nullptr)
    {
        for (int i = 0l; i < objectVector.size(); ++i)
        {
            if (objectVector[i]->GetObjectType() == EObjectType::eUiEntity)
            {
                objectVector[i]->OnEvent(&mouseEventUi);
            }
            else
            {
                objectVector[i]->OnEvent(&mouseEvent3D);
            }
        }
    }
    else
    {
        if (capturedObject->GetObjectType() == EObjectType::eUiEntity)
        {
            capturedObject->OnEvent(&mouseEventUi);
        }
        else
        {
            capturedObject->OnEvent(&mouseEvent3D);
        }

    }

    //기즈모를 먼저 판단해야한다.
    //그럼맵에 충돌월드에서도 기즈모를 유지하고있어야하나?
    //단 기즈모가 active되었다,select flag가 켜졌다는가정하에


    //if (collisionWorld->RayCastingFirst(SelectedObject, ray))
    if (map->RayCastingFirst(screenPos, SelectedObject))
    {


        //if (!SelectedObject->GetSelectAvailableFlag())
        //	return nullptr;


        if (!system.GetRuntimeModeState())
        {//edit모드이다.

            //그러면 기즈모를 띄운다.
            //그 오브젝트위치에
            EditorMap * map = static_cast<EditorMap*>(system.GetMap());
            
            Gizmo* gizmo = map->GetGizmo();
            if (!(SelectedObject == gizmo || gizmo->CheckChildHandleSelect(SelectedObject)))
            {
             //   system.OnGizmo(SelectedObject);
                gizmo->SetDestObject(SelectedObject);

                gizmo->OnGizmo();

            }
        }


        mouseEvent3D.SetGlobalFlag(false);
        mouseEventUi.SetGlobalFlag(false);

        if (capturedObject == nullptr || capturedObject == SelectedObject )
        {
            if (SelectedObject->GetObjectType() == EObjectType::eUiEntity)
            {
                SelectedObject->OnEvent(&mouseEventUi);
            }
            else
            {
                SelectedObject->OnEvent(&mouseEvent3D);
            }



            if (!system.GetRuntimeModeState())
            {//edit모드이다.

            // attribute controller에 전달
                EditorMap* map = static_cast<EditorMap*>(system.GetMap());

                Gizmo* gizmo = map->GetGizmo();
                if (!(SelectedObject == gizmo || gizmo->CheckChildHandleSelect(SelectedObject)))
                {
                    GameObjectSelectEvent* gameObjectselectEvent = new GameObjectSelectEvent;
                    gameObjectselectEvent->SetSelectedObject(SelectedObject);
                    EventDispatcher::SendEvent(gameObjectselectEvent, ESystemID::eAttributeMainSystem);
                }

            }
        }



        return nullptr;

    }
    else
    {
        //충돌하는게 없다.
        if (!system.GetRuntimeModeState())
        {//edit모드이다.
            EditorMap* map = static_cast<EditorMap*>(system.GetMap());

            Gizmo* gizmo = map->GetGizmo();
            //그러면 기즈모를 띄운다.
            //그 오브젝트위치에
           // system.OffGizmo();

            gizmo->SetDestObject(nullptr);

            gizmo->OffGizmo();


        }

    }

    return nullptr;

}

void Quad::GamePlaySystemDefaultState::HandleNotifyDragAndDropEvent(System & system , Event* event)
{

    NotifyDragAndDropEvent* dragDropEvent = (NotifyDragAndDropEvent*)event;

    ContentItem* contentItem = dragDropEvent->GetContentItem();
    int screenPosX = dragDropEvent->GetScreenPosX();
    int screenPosY = dragDropEvent->GetScreenPosY();
    bool lButtonUpState = dragDropEvent->GetLButtonUpState();

    switch (contentItem->GetEContentItemType())
    {
    case EContentItemType::eAsset:
    {
        Asset* asset = (Asset*)contentItem;
        HandleNotifyDragAndDropAsset(system,asset, screenPosX, screenPosY, lButtonUpState);


    }
    break;
    case EContentItemType::eLogicalFolder:
    {
        //아무일도 일어나지 않을것이다.
    }
    break;
    }










}

void Quad::GamePlaySystemDefaultState::HandleNotifyDragAndDropAsset(System& system,Asset * asset ,int screenPosX, int screenPosY,
    bool lButtonUpState)
{

    EditorMap* map =static_cast<EditorMap *>(system.GetMap());
    Camera* cameraUi = map->GetMainCamera(0);
    Camera* camera3D = map->GetMainCamera(1);



    Ray rayUi;
    Ray ray3D;


    //정확히는 client 영역에서의 위치이다.
    
    //ray를 계산하기전에 변환해야한다.
    HWND hwnd = system.GetWindow()->GetWindowHandle();
    POINT screenPos = { screenPosX,screenPosY };
    ScreenToClient(hwnd, &screenPos);

    if (cameraUi)
    {
        CameraHelper::CalculateRay({ float(screenPos.x),float(screenPos.y) }, map->GetViewPort(0), *cameraUi, true, rayUi);
    }

    if (camera3D)
    {
        CameraHelper::CalculateRay({ float(screenPos.x),float(screenPos.y) }, map->GetViewPort(1), *camera3D, true, ray3D);
    }

    



    
    //물체와 충돌하는가?
    //판단
    //충돌체공간에있는 충돌체들과 판정을 해야하겠지?
    //현재맵에있는
    //충돌월드에 도움을 부탁해야하지?


    EditorCollisionWorld* collisionWorld = static_cast<EditorCollisionWorld*>(map->GetCollisionWorld());
    Object* object =nullptr;
    //collisionWorld->RayCastingFirstNotIncludeEditorObject(object, ray);
    map->RayCastingFirstNotIncludeEditorObject({ (float)screenPos.x,(float)screenPos.y }, object);






    switch (asset->GetAssetType())
    {
    case EAssetType::eTexture:
    {
        if (lButtonUpState == false)
            break;
    

        Texture* texture = (Texture*)asset;
        //텍스처에도 여러종류가 있을것
        //일단은 diffuse map이라고하고적용시켜보자

        //ray판정을 해야한다.

        if (object == nullptr)
            break;

  
        //entity인가?
  
        //엔티티만 일단 생각해보자
        if (object->GetObjectType() != EObjectType::eEntity)
            break;

        //한 엔티티에는 여러개의 서브메시들이 존재할것이고 
        //정확이 어떤 submesh을 가리켰는가도 판단해야하지.  더 상세히 레이판정을 수행해야한다.//폴리곤단위로 수행해야할것.
    
        
        Entity* entity = (Entity*)object;
        if(entity->GetModel()!=nullptr)
        entity->GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetDiffuseMap(texture);

    }
    break;
    case EAssetType::eMesh:
    {
        //메시는 계속 메세지가 전달되어야하는데 여기가아니라 다른이벤트로 처리해야할거같다.
        //아니면 계속 보낼까? 
        //다른것들도 상황에따라 계속변할수있으니
        //일단 미루고
        //메시에대해서만 계속보내보자.

        //일단 화면안에 마우스가있는지확인부터해야하는데

        Mesh* mesh = (Mesh*)asset;
        if (!(system.GetClientWidth() < screenPos.x || screenPos.x < 0 || system.GetClientHeight() < screenPos.y || screenPos.y < 0))
        {

            DirectX::XMFLOAT3 origin = ray3D.GetOrigin();
            DirectX::XMFLOAT3 dir = ray3D.GetDir();

            //위치는 현재 레이위치에서 그방향으로 딱 일정크기 이동한곳으로 위치시킨다.
            DirectX::XMFLOAT3 entityPos;
            DirectX::XMStoreFloat3(&entityPos, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&origin), DirectX::XMVectorMultiply(DirectX::XMVectorSet(20.0F, 20.0F, 20.0F, 0.0F), DirectX::XMLoadFloat3(&dir))));


            //마우스가 화면안에 있다.
           
            if (mEnterSceneFirstFlag)
            {
                //처음드래그해서 처음으로 화면안을 들어왔다.
                //충돌체도 생성되니 제거할떄 그것도고려해라
                static int num = 0;
              //  mEntity =static_cast<Entity*>(map->CreateObject("Entity", 0));

                mEntity = Entity::Create(map, 1);
                mEntity->GetModel()->GetMeshComponent()->SetMesh(mesh);
                /*mEntity->SetSystem(&system);
                mEntity->Initialize();
                mEntity->SetName(mesh->GetName() + std::to_string(num));
                num++;*/

              //  system.AddEntity(mEntity);
                mEnterSceneFirstFlag = false;
                mMouseEnterSceneFlag = true;

                mEntity->SetActiveFlag(true);
                mEntity->SetDrawFlag(true);
                mEntity->SetEntireDrawFlag(true);
                mEntity->SetEntireSelectAvailableFlag(true);

                DragAndDropWindowController::SetShowState(false);
            }
            else
            {
                //처음들어온게아니다.
                if (mMouseEnterSceneFlag == false)
                {
                    //나갔다가 다시 들어오는데 딱 그 순간일때
                    mEntity->SetActiveFlag(true);
                    mEntity->SetDrawFlag(true);
                    mEntity->SetEntireDrawFlag(true);
                    mEntity->SetEntireSelectAvailableFlag(true);
                    mMouseEnterSceneFlag = true;
                    DragAndDropWindowController::SetShowState(false);
                }


            }

            mEntity->GetTransform().SetPositionWorld({ entityPos });


            if (lButtonUpState)
            {
                //안에서 up을했다
                //확정 

                //새로운 엔티티준비
               // mEntity = new Entity("");
                mEntity = nullptr;
                mEnterSceneFirstFlag = true;
                mMouseEnterSceneFlag = false;
            }



        }
        else
        {
            //마우스가 화면밖에 있다.
            if (mEnterSceneFirstFlag)
            {
                //한번도 화면안으로 마우스가 들어간적이없다면
                //아무것도 할게없다.

            }
            else
            {
                //한번이라도 들어갔다가 나왔다면 

                //그엔티티가 보여서는 안됨으로 비활성화한다.
                mEntity->SetActiveFlag(false);
                mEntity->SetDrawFlag(false);
                mEntity->SetEntireDrawFlag(false);
                mEntity->SetEntireSelectAvailableFlag(false);
                mMouseEnterSceneFlag = false;
                DragAndDropWindowController::SetShowState(true);

            }


            if (lButtonUpState)
            {
                //밖에서 up을했다.

                if (mEnterSceneFirstFlag)
                {

                    //아직 한번도 씬에 엔티티를 넣지않았다면 아무일도안일어난다.


                }
                else
                {
                    //한번이라도 화면에 엔티티가 들어가서 맵에 추가되었고
                    //그 엔티티를 제거한다.
                    system.RemoveObject(mEntity);
                    mEntity = nullptr;
                    

                   /* mEntity->SetActiveFlag(true);
                    mEntity->SetDrawFlag(true);
                    mEntity->SetEntireDrawFlag(true);*/
 
                }
                mEnterSceneFirstFlag = true;
                mMouseEnterSceneFlag = false;

                DragAndDropWindowController::SetShowState(false);

            }



        }



    }
    break;
    case EAssetType::eMaterial:
    {
        if (lButtonUpState == false)
            break;


        Material* material = (Material*)asset;

        if (object == nullptr)
            break;

        //엔티티만 일단 생각해보자
        if (object->GetObjectType() != EObjectType::eEntity)
            break;

        Entity* entity = (Entity*)object;
        //이것도 엔티티에는 여러서브메시가존재하니 더 상세히 레이판정을 수행해야한다.//폴리곤단위로 수행해야할것.



        //material을 고유하게 가져야하는데 값을 복사해야할것.

      entity->GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial = *material;
       
    }
    break;


    }











}
