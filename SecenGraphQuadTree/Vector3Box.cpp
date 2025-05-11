#include "Vector3Box.h"
#include"MeshManager.h"
#include"TextureManager.h"



void Quad::Vector3Box::Initialize()
{	
	
	BoxEntity::Initialize();



}
Quad::Vector3Box::Vector3Box(const std::string& name)
	:BoxEntity(name)
{

}
void Quad::Vector3Box::Update(float deltaTime)
{
	BoxEntity::Update(deltaTime);
}
void Quad::Vector3Box::OnEvent(Event* event)
{
}
;