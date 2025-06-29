#include"Component/UIMeshComponent.h"
#include"Collision/ColliderGenerator.h"
#include"ResourceManager/MeshManager.h"
#include<ResourceManager/TextureManager/TextureManager.h>
Quad::UIMeshComponent::UIMeshComponent()
{

	SetCollider(ColliderGenerator::CreateRectCollider());

	SetMesh(MeshManager::GetMesh("Rectangle"));

}

Quad::UIMeshComponent::~UIMeshComponent()
{
}

void Quad::UIMeshComponent::Start()
{
}

void Quad::UIMeshComponent::Update(float deltaTime)
{
}

void Quad::UIMeshComponent::SetMesh(Mesh* mesh)
{
	if (GetMesh() != nullptr)
		return;

	IMeshComponent::SetMesh(mesh);

}

void Quad::UIMeshComponent::Serialize(const std::string& tag)
{
	IMeshComponent::Serialize(tag);

}

void Quad::UIMeshComponent::DeSerialize(const std::string& tag)
{
	IMeshComponent::DeSerialize(tag);



}

void Quad::UIMeshComponent::SetWidthLocal(float width)
{
	Transform  & transform = GetTransform();
	DirectX::XMFLOAT3 scale = transform.GetScaleLocal();
	scale.x = width;
	transform.SetScaleLocal(scale);

}

void Quad::UIMeshComponent::SetHeightLocal(float height)
{
	Transform& transform = GetTransform();

	DirectX::XMFLOAT3 scale = transform.GetScaleLocal();
	scale.y = height;
	transform.SetScaleLocal(scale);

}

float Quad::UIMeshComponent::GetWidthLocal() const
{
	Transform & transform =	GetTransform();

	return transform.GetScaleLocal().x;
	
}

float Quad::UIMeshComponent::GetHeightLocal() const
{
	Transform& transform = GetTransform();

	return transform.GetScaleLocal().y;
}

void Quad::UIMeshComponent::SetWidthHeightLocal(float width, float height)
{
	Transform& transform = GetTransform();
	DirectX::XMFLOAT3 scale = transform.GetScaleLocal();
	scale.x = width;
	scale.y = height;

	transform.SetScaleLocal(scale);


}

float Quad::UIMeshComponent::GetWidthWorld() const
{
	return GetScaleWorld().x;
}

float Quad::UIMeshComponent::GetHeightWorld() const
{
	return GetScaleWorld().y;
}




void Quad::UIMeshComponent::SetColorItensity(const DirectX::XMFLOAT3& intensity)
{
	mSubMeshVector[0].mModelMaterial.SetColorIntensity(intensity);
}

void Quad::UIMeshComponent::SetColor(const DirectX::XMFLOAT3& color)
{
	mSubMeshVector[0].mModelMaterial.SetColor(color);
}

DirectX::XMFLOAT3 Quad::UIMeshComponent::GetColor() const
{
	return mSubMeshVector[0].mModelMaterial.GetColor();
}

void Quad::UIMeshComponent::SetTexture(Texture* texture)
{
	mSubMeshVector[0].mModelMaterial.SetDiffuseMap(texture);
}
void Quad::UIMeshComponent::SetTexture(const char* textureName)
{
	mSubMeshVector[0].mModelMaterial.SetDiffuseMap(TextureManager::GetTexture(textureName));

}
void Quad::UIMeshComponent::SetEffect(const std::string& effectName)
{

	mSubMeshVector[0].mModelMaterial.SetEffectName(effectName);

}