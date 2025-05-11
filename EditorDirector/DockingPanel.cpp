#include "EditorDirector/DockingPanel.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"System/WindowDockingSystem.h"
Quad::DockingPanel::DockingPanel(const std::string& name)
	:UiEntity(name)
{
}

void Quad::DockingPanel::Initialize()
{

	UiEntity::Initialize();

	/*GetModel()->GetMeshComponent()->SetMesh(MeshManager::GetMesh("Rectangle"));

	GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetDiffuseMap(TextureManager::GetTexture("DockingPanel.png"));
	GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetEffectName("Docking.effect");*/

}

Quad::DockingPanel* Quad::DockingPanel::Create(Map * map,int mapLayerIndex)
{

	DockingPanel * panel = static_cast<DockingPanel*>(map->CreateObject("DockingPanel", mapLayerIndex));
//	panel->Initialize();
	panel->InitCreating();
	
	return panel;
}

void Quad::DockingPanel::Update(float deltaTime)
{
	UiEntity::Update(deltaTime);
}

void Quad::DockingPanel::OnEvent(Event* event)
{
	UiEntity::OnEvent(event);

	const std::string& eventName = event->GetName();
	


}

void Quad::DockingPanel::SetSize(float width, float height)
{
	mWidth = width;
	mHeight = height;

	GetTransform().SetScaleLocal({ width,height,1.0f });

}

float Quad::DockingPanel::GetWidth() const
{
	return mWidth;
}

float Quad::DockingPanel::GetHeight() const
{
	return mHeight;
}

void Quad::DockingPanel::SetPosition(float x, float y, float z)
{
	GetTransform().SetPositionLocal({ x,y,z });
}

void Quad::DockingPanel::InitCreating()
{
	UiEntity::InitCreating();
	
	Model* model = GetModel();
	MeshComponent* meshComponent = model->GetMeshComponent();

	//meshComponent->SetMesh(MeshManager::GetMesh("Rectangle"));
	meshComponent->GetSubMesh()[0].mModelMaterial.SetDiffuseMap(TextureManager::GetTexture("DockingPanel.png"));
	meshComponent->GetSubMesh()[0].mModelMaterial.SetEffectName("Docking.effect");
}

