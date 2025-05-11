//#include "WorldEditUiEntity.h"
//#include"BoundaryRect.h"
//#include"MeshManager.h"
//#include"TextureManager.h"
//#include"Map.h"
//#include"BoundaryRect.h"
//#include"EventTable.h"
//#include"MouseEvent.h"
//#include"ResizeEvent.h"
//#include"System.h"
//
//
//Quad::WorldEditUiEntity::WorldEditUiEntity(const std::wstring& name)
//    :UiEntity(name)
//{
//}
//
//Quad::WorldEditUiEntity::~WorldEditUiEntity()
//{
//    delete mBoundaryRect;
//}
//
//
//
//void Quad::WorldEditUiEntity::Initialize()
//{
//    UiEntity::Initialize();
//    CreateBoundrayRect();
//
//    System * system =   GetSystem();
//
//    system->RegisterEntityToEvent("Select", this);
//    system->RegisterEntityToEvent("LButtonDown", this);
//    system->RegisterEntityToEvent("MouseMove", this);
//    system->RegisterEntityToEvent("LButtonUp", this);
//}
//
//void Quad::WorldEditUiEntity::SetClassId(const std::wstring& classId)
//{
//    mClassId = classId;
//}
//
//const std::wstring& Quad::WorldEditUiEntity::GetClassId() const
//{
//    return mClassId;
//    // TODO: 여기에 return 문을 삽입합니다.
//}
//
//void Quad::WorldEditUiEntity::Update(float deltaTime)
//{
//    UiEntity::Update(deltaTime);
//
//
//
//}
//
//void Quad::WorldEditUiEntity::OnEvent(Event* event)
//{
//    std::string eventName = event->GetName();
//
//
//    if (eventName == "LButtonDown")
//    {
//        SetLButtonDownState(true);
//    }
//    else if (eventName == "LButtonUp")
//    {
//        SetLButtonDownState(false);
//
//    }else if (eventName == "Select")
//    {//클릭되어지는 순간부터
//        SetSelectState(true);
//        SetEntireDrawFlag(true);
//        if (mBoundaryRect != nullptr)
//            mBoundaryRect->OnEvent(event);
//
//        //그럼경계상자는 내부에서 drawflag를 키고 ~~ 등등
//        // 그러고 이제 다른 이벤트들을 받을수있고 클릭될수있고, 그리고 적절하게 그에따라 사이즈조절을 수행하는거다
//        //
//        //~적절한처리
//        
//    }
//    else if (eventName == "ReleaseSelect")
//    {
//        SetSelectState(false);
//        if (mBoundaryRect != nullptr)
//            mBoundaryRect->OnEvent(event);
//
//    }
//    else if (eventName == "MouseMove")
//    {
//        HandleMouseMoveEvent(event);
//
//
//    }
//    else if (eventName == "Resize")
//    {
//        HandleResizeEvent(event);
//    }
//
//
//}
//
//void Quad::WorldEditUiEntity::SetBoundaryRect(BoundaryRect* boundaryRect)
//{
//    mBoundaryRect = boundaryRect;
//}
//
//Quad::BoundaryRect* Quad::WorldEditUiEntity::GetBoundaryRect() const
//{
//    return mBoundaryRect;
//}
//
//
//bool Quad::WorldEditUiEntity::AddAttribute(const std::wstring& name,
//    const Attribute& attribute)
//{
//
//  std::pair< std::map<std::wstring,Attribute>::iterator,bool>  ret
//      = mAttributeMap.insert({ name,attribute });
//
//  return ret.second;
//
//}
//
//void Quad::WorldEditUiEntity::CreateBoundrayRect()
//{
//
//    Transform& worldEditUiEntityTransform = GetTransform();
//
//    mBoundaryRect = new BoundaryRect(L"");
//    mBoundaryRect->Initialize(worldEditUiEntityTransform.GetWidthWorld(), worldEditUiEntityTransform.GetHeightWorld());
//
//    DirectX::XMFLOAT3 boundaryRectPos = GetTransform().GetPositionWorld();
//    boundaryRectPos.z -= 1.0f;
//    mBoundaryRect->GetTransform().SetPositionLocal(boundaryRectPos);
//    
//    AddChildObject(mBoundaryRect);
//    mBoundaryRect->SetDstEntity(this);
//
//
//}
//
//void Quad::WorldEditUiEntity::HandleMouseMoveEvent(Event* event)
//{
//    
//    MouseEvent* mouseEvent = (MouseEvent*)event;
//    
//    //선택된상태이면서 엔티티를 클릭한 상태일때 움직이면
//    //엔티티는 마우스를 따라 움직인다.
//    /*if (GetSelectState() && GetLButtonDownState())
//    {
//        INT distanceX = mouseEvent->GetScreenPositionX() - mouseEvent->GetPreScreenPositionX();
//        INT distanceY = mouseEvent->GetScreenPositionY() - mouseEvent->GetPreScreenPositionY();
//
//        Transform& transform = GetTransform();
//        DirectX::XMFLOAT3 positionLocal = transform.GetPositionLocal();
//        positionLocal.x += distanceX;
//        positionLocal.y -= distanceY;
//        GetTransform().SetPositionLocal(positionLocal);
//
//    }*/
//
//
//
//
//}
//
//void Quad::WorldEditUiEntity::HandleResizeEvent(Event* event)
//{
//    ResizeHandleEvent* resizeEvent = (ResizeHandleEvent*)event;
//
//    DirectX::XMFLOAT3 newSize = resizeEvent->GetNewSize();
//    Transform& transform = GetTransform();
//    float preWidth = transform.GetWidthWorld();
//    float preHeight = transform.GetHeightWorld();
//    float preDepth = transform.GetDepthWorld();
//
//    int dir = resizeEvent->GetDirection();
//    //최소 사이즈를 정해야한다.
//    DirectX::XMFLOAT3 posLocal = GetTransform().GetPositionLocal();
//    
//    
//    switch (dir)
//    {
//    case 0:     //left
//    {
//        float distance = newSize.x - preWidth;
//        posLocal.x -= distance / 2;
//    }
//        break;
//
//    case 1:     //right
//    {
//        float distance = newSize.x - preWidth;
//        posLocal.x += distance/2;
//    }
//        break;
//    case 2:     //top   
//    {
//        float distance = newSize.y - preHeight;
//        posLocal.y += distance/2;
//    }
//
//        break;
//
//    case 3:         //bottom
//    {
//        float distance = newSize.y - preHeight;
//        posLocal.y -= distance/2;
//    }
//        break;
//    }
//
//    
//    GetTransform().SetPositionLocal(posLocal);
//    GetTransform().SetWidthWorld(newSize.x);
//    GetTransform().SetHeightWorld(newSize.y);
//    GetTransform().SetDepthWorld(newSize.z);
//
//}
//
//
//
//namespace Quad
//{
//
//
//
//}