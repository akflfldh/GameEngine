#include "Object/Line/Grid.h"
#include"Map/Map.h"

#include"ObjectFactory/LineFactory.h"
#include"Component/ColliderComponent.h"
#include"Collision/BoxCollider.h"

Quad::Grid* Quad::Grid::Create(Map* map, int mapLayerIndex, int rowNum, int columnNum, float cellWidth, float cellHeight)
{
	Grid* grid = static_cast<Grid*>(map->CreateObject("Grid", mapLayerIndex));
	//grid->Initialize();
	grid->InitCreating(rowNum, columnNum,cellWidth,cellHeight);



	return grid;
}

void Quad::Grid::Initialize()
{
	//처음dirty플래그를 켜서 씬그래프에서 update시 collider에 올바르게 속성값들이 반영되도록한다.
	//dirty플래그가 꺼져있으면 업데이트되지않기때문이다.
	GetTransform().SetDirtyFlag(true);
	

	




}

void Quad::Grid::DefaultCreatingInitialize()
{
}

void Quad::Grid::OnEvent(Event* event)
{
}

void Quad::Grid::Update(float deltaTime)
{
}

void Quad::Grid::EndUpdate(float deltaTime)
{
}

//void Quad::Grid::UpdateCollider(float deltaTime)
//{
//}

void Quad::Grid::Serialize()
{
}

void Quad::Grid::DeSerialize()
{
}

void Quad::Grid::InitCreating(int rowNum, int columnNum, float cellWidth, float cellHeight)
{
	LineFactory::ChangeGrid(this, rowNum, columnNum, cellWidth, cellHeight);

	ColliderComponent* colliderComponent = GetModel()->GetColliderComponent();
	BoxCollider* boxCollider = static_cast<BoxCollider*>(colliderComponent->GetCollider());

	boxCollider->SetWidthX(rowNum * cellWidth);
	boxCollider->SetHeightY(10);
	boxCollider->SetDepthZ(columnNum * cellHeight);

}

Quad::Grid::Grid(const std::string& name)
	:LineBase(name)
{
	SetEffectName("Line.effect");

	Model* model = GetModel();
	model->AddComponernt(EComponentType::eColliderComponent);

	ColliderComponent* colliderComponent = model->GetColliderComponent();
}
