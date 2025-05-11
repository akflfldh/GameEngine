#include "Object/Line/Line.h"


#include"Component/ColliderComponent.h"
#include"Collision/BoxCollider.h"

Quad::Line::Line(const std::string & name)
	:LineBase(name)
{
	SetEffectName("Line.effect");
	LineModelSubMesh* modelSubMesh = GetModelSubMesh();
	modelSubMesh->mIndexRange = { 0,6 };
	modelSubMesh->mVertexNum = 2;
	modelSubMesh->mVertexOffset = 0;


	Model* model = GetModel();
	model->AddComponernt(EComponentType::eColliderComponent);

	ColliderComponent * colliderComponent =	model->GetColliderComponent();

	BoxCollider * collider= static_cast<BoxCollider*>(GetModel()->GetColliderComponent()->GetCollider());
	

}


void Quad::Line::Initialize()
{

}

void Quad::Line::DefaultCreatingInitialize()
{
}

void Quad::Line::OnEvent(Event* event)
{


}

void Quad::Line::Update(float deltaTime)
{

}

void Quad::Line::EndUpdate(float deltaTime)
{
}

void Quad::Line::SetStartVertexPosition(const DirectX::XMFLOAT3& pos)
{

	GetTransform().SetDirtyFlag(true);
	LineVertex vertex;
	vertex.mPos = pos;
	mStartPosition = pos;
	UploadBuffer* vertexBuffer = GetVertexBuffer();




	vertexBuffer->CopyData(0, &pos, sizeof(vertex));
 
	BoxCollider* collider = static_cast<BoxCollider*>(GetModel()->GetColliderComponent()->GetCollider());
	collider->SetMinMaxVertex(mStartPosition, mEndPosition);

}

void Quad::Line::SetEndVertexPosition(const DirectX::XMFLOAT3& pos)
{
	GetTransform().SetDirtyFlag(true);
	LineVertex vertex;
	vertex.mPos = pos;
	mEndPosition = pos;
	UploadBuffer* vertexBuffer = GetVertexBuffer();

	vertexBuffer->CopyData(1, &pos, sizeof(vertex));

	BoxCollider* collider = static_cast<BoxCollider*>(GetModel()->GetColliderComponent()->GetCollider());
	collider->SetMinMaxVertex(mStartPosition, mEndPosition);
}

const DirectX::XMFLOAT3& Quad::Line::GetStartVertexPosition() const
{
	return mStartPosition;
	// TODO: 여기에 return 문을 삽입합니다.
}

const DirectX::XMFLOAT3& Quad::Line::GetEndVertexPosition() const
{
	return mEndPosition;
	// TODO: 여기에 return 문을 삽입합니다.
}





void Quad::Line::Serialize()
{
}

void Quad::Line::DeSerialize()
{
}




//void Quad::Line::UpdateCollider(float deltaTime)
//{
//}
//
