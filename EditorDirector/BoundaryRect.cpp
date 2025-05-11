#include"BoundaryRect.h"
#include"ResizeHandle.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"Map/Map.h"
#include"BoundaryStick.h"
#include"Event/EventTable.h"

void Quad::BoundaryRect::Initialize()
{

   // SetSizeInitOption(false);

    UiEntity::Initialize();

}

void Quad::BoundaryRect::InitCreating(float width, float height)
{

    UiEntity::InitCreating();
    SetWidth(width);
    SetHeight(height);
    SetEffect("DefaultUi.effect");



    MeshComponent* meshComponent = static_cast<MeshComponent*>(GetModel()->GetComponent(EComponentType::eMeshComponent));

    meshComponent->SetTextureToAllSubMesh(TextureManager::GetTexture("Red.png"));
    //항상 그냥 정사각형 rect mesh가 들어와야하는데 이거는 팩토리패턴으로 처리해야하는가.
    DirectX::XMFLOAT3 boundaryPositionWorld = GetTransform().GetPositionWorld();
    DirectX::XMFLOAT3 boundaryScaleWorld = GetTransform().GetScaleWorld();


    //BoundaryRect는 그냥 껍데기
    //전체 draw on off 개별 flag,
    //update에는 active flag
    SetEntireDrawFlag(true);
    SetActiveFlag(true);
    SetDrawFlag(false);

    Map* map = GetMap();
    int mapLayerIndex = GetMapLayerID();
    //사각막대4개를 rect메시를 가지는 자식엔티티로 개별적으로 만들어야겠다 
    //그 막대안에 각 resizehandle이 자식으로 하나씩 존재하는것이다.
    const std::string& boundaryRectName = GetName();
    for (int i = 0; i < 4; ++i)
    {
        // mBoundaryStick[i] = new BoundaryStick(boundaryRectName+L"_BoundaryStick" + std::to_wstring(i));
        //mBoundaryStick[i] = new BoundaryStick("");
     //   mBoundaryStick[i] = static_cast<BoundaryStick*>(map->CreateObject("BoundaryStick"));
        mBoundaryStick[i] = BoundaryStick::Create(map, mapLayerIndex,i);
        //  mBoundaryStick[i]->SetSystem(GetSystem());
       // mBoundaryStick[i]->Initialize(i);
        mBoundaryStick[i]->GetTransform().SetIndependentScaleFlag(true);
        mBoundaryStick[i]->SetDrawFlag(false);
        mBoundaryStick[i]->SetEntireDrawFlag(false);
        mBoundaryStick[i]->SetActiveFlag(false);

    }

    //stick 위치와 크기조정
   //boundaryrect의 각 모서리 중앙에 위치해야한다.
    //left
    mBoundaryStick[0]->GetTransform().SetPositionLocal(
        { boundaryPositionWorld.x - 0.5f * boundaryScaleWorld.x,boundaryPositionWorld.y,boundaryPositionWorld.z - 1.0f });
    mBoundaryStick[0]->GetTransform().SetScaleLocal({ 1.0f,1.0f,1.0f });
    mBoundaryStick[0]->GetTransform().SetRotationLocal({ 0.0f,0.0f,90.0f });

    //right
    mBoundaryStick[1]->GetTransform().SetPositionLocal(
        { boundaryPositionWorld.x + 0.5f * boundaryScaleWorld.x,boundaryPositionWorld.y,boundaryPositionWorld.z - 1.0f });
    mBoundaryStick[1]->GetTransform().SetScaleLocal({ 1.0f,1.0f,1.0f });
    mBoundaryStick[1]->GetTransform().SetRotationLocal({ 0.0f,0.0f,-90.0f });
    //left와 top을 회전시켜야한다 


    //top
    mBoundaryStick[2]->GetTransform().SetPositionLocal(
        { boundaryPositionWorld.x,boundaryPositionWorld.y + 0.5f * boundaryScaleWorld.y,boundaryPositionWorld.z - 1.0f });
    mBoundaryStick[2]->GetTransform().SetScaleLocal({ 1.0f,1.0f,1.0f });

    //bottom
    mBoundaryStick[3]->GetTransform().SetPositionLocal(
        { boundaryPositionWorld.x,boundaryPositionWorld.y - 0.5f * boundaryScaleWorld.y,boundaryPositionWorld.z - 1.0f });
    mBoundaryStick[3]->GetTransform().SetScaleLocal({ 1.0f,1.0f,1.0f });
    mBoundaryStick[3]->GetTransform().SetRotationLocal({ 0.0f,0.0f,180.0f });


    //설정이끝난후 
    for (int i = 0; i < 4; ++i)
        AddChildObject(mBoundaryStick[i]);







}


Quad::BoundaryRect* Quad::BoundaryRect::Create(Map* map, int mapLayerIndex, float width, float height)
{
    BoundaryRect* ob = static_cast<BoundaryRect*>(map->CreateObject("BoundaryRect", mapLayerIndex));
    ob->Initialize();
    ob->InitCreating(width,height);

    return ob;
}

Quad::BoundaryRect::BoundaryRect(const std::string& name)
    :UiEntity(name)
{
    SetIDState(false);
}

void Quad::BoundaryRect::Update(float deltaTime)
{
    UiEntity::Update(deltaTime);

     
    DirectX::XMFLOAT3 scale = GetTransform().GetScaleWorld();


    Transform& transform = GetTransform();
    float width = transform.GetWidthWorld();
    float height = transform.GetHeightWorld();

    mBoundaryStick[0]->GetTransform().SetWidthWorld(height);
    mBoundaryStick[1]->GetTransform().SetWidthWorld(height);
    mBoundaryStick[2]->GetTransform().SetWidthWorld(width);
    mBoundaryStick[3]->GetTransform().SetWidthWorld(width);
  

    mBoundaryStick[0]->GetTransform().SetPositionLocal({-0.5f*scale.x ,0.0f,-1.0f});
    mBoundaryStick[1]->GetTransform().SetPositionLocal({ 0.5f*scale.x,0.0f,-1.0f });
    mBoundaryStick[2]->GetTransform().SetPositionLocal({0.0,0.5f*scale.y,-1.0f});
    mBoundaryStick[3]->GetTransform().SetPositionLocal({ 0.0f,-0.5f * scale.y,-1.0f });
    

}

void Quad::BoundaryRect::OnEvent(Event* event)
{

    std::string eventName = event->GetName();

    if (eventName == "Select")
    {
        SetActiveFlag(true);
        SetEntireDrawFlag(true);
        //SetDrawFlag(true);
        for (int i = 0; i < 4; ++i)
            mBoundaryStick[i]->OnEvent(event);
    }
    else if (eventName == "ReleaseSelect")
    {
        SetEntireDrawFlag(false);
    /*    for (int i = 0; i < 4; ++i)
            mBoundaryStick[i]->OnEvent(event);*/

    }



}

void Quad::BoundaryRect::SetPositionLocal(DirectX::XMFLOAT3 pos)
{
  /*  for (int i = 0; i < 4; ++i)
    {
        mResizeHandle[i]->GetTransform().SetPositionLocal()
    }*/




}

void Quad::BoundaryRect::SetWidth(float width)
{
    mWidth = width;

    DirectX::XMFLOAT3 scale =GetTransform().GetScaleLocal();
    scale.x = width;
    GetTransform().SetScaleLocal(scale);
}

void Quad::BoundaryRect::SetHeight(float height)
{
    mHeight = height;
    DirectX::XMFLOAT3 scale = GetTransform().GetScaleLocal();
    scale.y = mHeight;
    GetTransform().SetScaleLocal(scale);

}

float Quad::BoundaryRect::GetWidth() const
{
    return mWidth;
}

float Quad::BoundaryRect::GetHeight() const
{
    return mHeight;
}

void Quad::BoundaryRect::SetDstEntity(UiEntity* entity)
{
    
    mDstEntity = entity;
    for (int i = 0; i < 4; ++i)
    {
        if(mBoundaryStick[i]!=nullptr)
             mBoundaryStick[i]->SetDstEntity(entity);
    }

}

