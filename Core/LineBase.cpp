#include "Object/Line/LineBase.h"
#include"Component/ColliderComponent.h"

Quad::LineBase::LineBase(const std::string& name, EObjectType objectType)
	:Object(name, objectType)
{
	mModelSubMesh.mModelMaterial.SetColor(DirectX::XMFLOAT3{1.0f,0.0f,1.0f });


	//Model* model = GetModel();
//	model->AddComponernt(EComponentType::eColliderComponent);

//ColliderComponent * colliderComponent =	model->GetColliderComponent();
}

Quad::LineBase::~LineBase()
{
	//delete mUploadVertexBuffer;

}


void Quad::LineBase::SetVertexBuffer(UploadBuffer* vertexBuffer)
{
	mUploadVertexBuffer = vertexBuffer;
}

D3D12_GPU_VIRTUAL_ADDRESS Quad::LineBase::GetVertexBufferGpuVirtualAddress() const
{
	return mUploadVertexBuffer->GetGpuVirtualAddress();
}

D3D12_VERTEX_BUFFER_VIEW Quad::LineBase::GetVertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = GetVertexBufferGpuVirtualAddress();
	vbv.StrideInBytes = sizeof(LineVertex);
	vbv.SizeInBytes = GetVertexNum() * vbv.StrideInBytes;
	return vbv;


}



void Quad::LineBase::SetEffectName(const std::string& name)
{
	mEffectName = name;
}

const std::string& Quad::LineBase::GetEffectName() const
{
	return mEffectName;
	// TODO: 여기에 return 문을 삽입합니다.
}

size_t Quad::LineBase::GetVertexNum() const
{
	return 	mModelSubMesh.mVertexNum;
}

void Quad::LineBase::SetVertexNum(size_t num)
{
	mModelSubMesh.mVertexNum = num;
}

//const Quad::LineBaseMaterial& Quad::LineBase::GetMaterial() const
//{
//	return	mMaterial;
//	// TODO: 여기에 return 문을 삽입합니다.
//}
//
//Quad::LineBaseMaterial& Quad::LineBase::GetMaterial()
//{
//
//	return mMaterial;
//	// TODO: 여기에 return 문을 삽입합니다.
//}

const Quad::LineModelSubMesh* Quad::LineBase::GetModelSubMesh() const
{
	return &mModelSubMesh;
}

Quad::LineModelSubMesh* Quad::LineBase::GetModelSubMesh()
{
	return &mModelSubMesh;
}

bool Quad::LineBase::InnerDetectCollsion(Collider* colliderA, Collider* colliderB)
{
	return false;
}

bool Quad::LineBase::InnerDetectRayHit( const Ray& ray)
{
	return false;
}

Quad::UploadBuffer* Quad::LineBase::GetVertexBuffer() const
{
	return mUploadVertexBuffer;
}

