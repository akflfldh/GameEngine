#include "Collision/ColliderGenerator.h"
#include"Asset/Mesh/Mesh.h"
#include"ResourceManager/MeshManager.h"
#include"Collision/BoxCollider.h"
#include"Collision/SphereCollider.h"
#include"Collision/OrientedCollider.h"
#include"Collision/FrustumCollider.h"
#include"Buffer/UploadBuffer.h"
#include"Utility/Utility.h"
#include"GraphicCommand.h"
#include"Object/Camera/Camera.h"
#include"Collision/CameraBoxCollider.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"Collision/CameraFrustumCollider.h"
#include"ResourceManager/MaterialManager/MaterialManager.h"
#include"Asset/Mesh/StaticMesh.h"
#include"Asset/Material/ModelMaterial/ModelMaterial.h"
#include"Object/Camera/OrthogoanlCamera.h"
#include"Object/Camera/FrustumCamera.h"
#include"Allocator/BinnedAllocator.h"

#include"Collision/RectCollider.h"
#include"Collision/UiCollider.h"

Quad::BinnedAllocator* GetColliderBinnedAllocator(unsigned int size)
{

	static Quad::BinnedAllocator allocator16(16);
	static Quad::BinnedAllocator allocator32(32);
	static Quad::BinnedAllocator allocator48(48);
	static Quad::BinnedAllocator allocator64(64);
	static Quad::BinnedAllocator allocator80(80);
	static Quad::BinnedAllocator allocator96(96);
	static Quad::BinnedAllocator allocator128(128);
	static Quad::BinnedAllocator allocator256(256);
	static Quad::BinnedAllocator allocator512(512);
	static Quad::BinnedAllocator allocator1024(1024);
	static Quad::BinnedAllocator allocator2048(2048);


	static std::vector<int> sizeVector = { 16,32,48,64,80,96,128,256,512,1024,2048 };
	static std::vector<Quad::BinnedAllocator*> allocatorVector = { &allocator16,&allocator32,&allocator48,&allocator64,&allocator80,&allocator96,
	&allocator128,&allocator256,&allocator512,&allocator1024,&allocator2048 };


	for (int i = 0; i < sizeVector.size(); ++i)
	{

		int allocSize = sizeVector[i];
		if (size <= allocSize)
		{
			return allocatorVector[i];
		}
	}
	throw std::bad_alloc();

}


Quad::ColliderGenerator::ColliderGenerator()
	:mRectColliderAllocator(10000), mSplineColliderAllocator(1000)
{
	mColliderSizeTable["BoxCollider"] = sizeof(BoxCollider);
	mColliderSizeTable["BoxCollider"] = sizeof(BoxCollider);

	InitColliderDefaultConstructor();
}

void Quad::ColliderGenerator::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device ,GraphicCommand * graphicsCommandObject)
{
	mDevice = device;
	mGraphicsCommandObject = graphicsCommandObject;
	InitBoxIndexBuffer();
	InitFrustumIndexBuffer();

	mRectColliderAllocator.Initialize();
	mSplineColliderAllocator.Initialize();
}

Quad::Collider* Quad::ColliderGenerator::CreateBoxCollider(Mesh* mesh)
{
	auto instance = GetInstance();

	std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> minMaxPoint = mesh->GetMinMaxPoint();

	DirectX::XMFLOAT3 min = minMaxPoint.first;
	DirectX::XMFLOAT3 max = minMaxPoint.second;


	return CreateBoxCollider(min, max);

}

Quad::Collider* Quad::ColliderGenerator::CreateBoxCollider(const DirectX::XMFLOAT3& minPoint, const DirectX::XMFLOAT3& maxPoint)
{
	

	//BoxCollider* collider = new BoxCollider;
	BoxCollider* collider = new(GetColliderBinnedAllocator(sizeof(BoxCollider))->Alloc())BoxCollider;
	DirectX::BoundingBox boxDx;
	DirectX::XMVECTOR pt1 = DirectX::XMLoadFloat3(&minPoint);
	DirectX::XMVECTOR pt2 = DirectX::XMLoadFloat3(&maxPoint);
	DirectX::XMVECTOR defaultPt1 = DirectX::XMVectorSet(0.5, 0.5, 0.5, 1.0F);
	DirectX::XMVECTOR defaultPt2 = DirectX::XMVectorSet(-0.5, -0.5, -0.5, 1.0F);
	DirectX::BoundingBox::CreateFromPoints(boxDx, defaultPt1, defaultPt2);



	//default world matrix 가필요해 
	//처음충돌체mesh와 이 엔티티의 메시가 자치하는 공간에대해서 일치시키여함으로
	DirectX::XMVECTOR center = DirectX::XMVectorDivide(DirectX::XMVectorAdd(pt1, pt2), DirectX::XMVectorSet(2.0f, 2.0f, 2.0f, 1.0f));
	DirectX::XMVECTOR size = DirectX::XMVectorSubtract(pt2, pt1);	//,x,y,z,축에평행한모서리들의길이

	//기본 정육면체 메시의 사이즈는 중심은 원점이고, X Y ,Z축에 평행한 모서리들의 길이는 1.0이다.
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(center);
	DirectX::XMMATRIX scalingMatrix = DirectX::XMMatrixScalingFromVector(size);

	DirectX::XMFLOAT4X4 defaultMatrix;
	DirectX::XMStoreFloat4x4(&defaultMatrix, DirectX::XMMatrixMultiply(scalingMatrix, translationMatrix));


	collider->SetDefaultMatrix(defaultMatrix);
	Mesh* cubeMesh = MeshManager::GetMesh("Cube");
	collider->Initialize(boxDx, cubeMesh);
	//TextureManager::GetTexture("BoxCollider.png")

//	collider->GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetDiffuseMap(TextureManager::GetTexture("BoxCollider.png"));

	return collider;
}

Quad::Collider * Quad::ColliderGenerator::CreateFrustumCollider(FrustumCamera* caerma)
{
	//FrustumCollider* collider = new FrustumCollider;
	FrustumCollider* collider = new (GetColliderBinnedAllocator(sizeof(FrustumCollider))->Alloc())FrustumCollider;
	
	DirectX::XMFLOAT4X4 projMatrixF = caerma->GetProjMatrix();

	DirectX::BoundingFrustum frustum;
	Mesh* cubeMesh = MeshManager::GetMesh("Cube");



	DirectX::BoundingFrustum::CreateFromMatrix(frustum, DirectX::XMLoadFloat4x4(&projMatrixF));
	collider->Initialize(cubeMesh,frustum);

	return collider;

}

Quad::CameraFrustumCollider* Quad::ColliderGenerator::CreateCameraFrustumCollider(FrustumCamera* camera)
{

	auto instance = GetInstance();
//	CameraFrustumCollider* collider = new CameraFrustumCollider;
	CameraFrustumCollider* collider = new (GetColliderBinnedAllocator(sizeof(CameraFrustumCollider))->Alloc())CameraFrustumCollider;


	//vertex륾담는 uploadBuffer생성.
	
	UploadBuffer* vertexUploadBuffer = new UploadBuffer(instance->mDevice, sizeof(StaticVertex), 8, false);

	UploadFrustumVertexVector(camera, vertexUploadBuffer);


	//일반적으로 유저가 메시륾 만들면 화면에 보이지.
	//이거는 여기서 직접만들어야하나?
	//MeshManager::CreateMesh()
	StaticMesh * mesh = new StaticMesh;
	mesh->SetMeshType(EMeshType::eStaticMesh);

	mesh->SetIndexBuffer(instance->GetFrustumIndexBuffer());
//	mesh->SetVertexBuffer()
	mesh->SetVertexUploadBuffer(vertexUploadBuffer);
	mesh->SetVertexNum(8);
	mesh->SetIndexNum(36);

	std::vector<SubMesh> subMeshVector(1);
	subMeshVector[0].mIndexRange = { 0,36 };
	subMeshVector[0].mVertexNum = 8;
	subMeshVector[0].mVertexOffset = 0;
	subMeshVector[0].mMesh = (Mesh*)mesh;
	subMeshVector[0].mMaterialPointer = MaterialManager::GetMaterial("Default");
	mesh->SetSubMeshVector(std::move(subMeshVector));



	// 
	// 
	//mesh생성

	//update시에 vertex를 담은 uploadBuffer에 새로운 vertex upload
	collider->Initialize(camera, mesh);

	return collider;
}

Quad::CameraBoxCollider* Quad::ColliderGenerator::CreateBoxCameraCollider(OrthogoanlCamera* camera)
{
	auto instance = GetInstance();
	//카메라의 직교투영
	float width = camera->GetViewWidth();
	float height = camera->GetViewHeight();
	float nearPlane = camera->GetNearPlane();
	float farPlane = camera->GetFarPlane();
	//시야공간을 기준으로하는 boxCollider
	//CameraBoxCollider* collider = new CameraBoxCollider;
	CameraBoxCollider* collider = new((CameraBoxCollider*)GetColliderBinnedAllocator(sizeof(CameraBoxCollider))->Alloc())CameraBoxCollider;
	DirectX::BoundingBox boxDx;
	DirectX::XMVECTOR pt1 = DirectX::XMVectorSet(-width / 2, -height / 2, nearPlane, 1.0f);
	DirectX::XMVECTOR pt2 = DirectX::XMVectorSet(width / 2, height / 2, farPlane, 1.0f);

	DirectX::XMVECTOR  defaultPt1 = DirectX::XMVectorSet(-0.5, -0.5, -0.5, 1.0F);
	DirectX::XMVECTOR  defaultPt2 = DirectX::XMVectorSet(0.5, 0.5, 0.5, 1.0F);
	DirectX::BoundingBox::CreateFromPoints(boxDx, defaultPt1, defaultPt2);



	//default world matrix 가필요해 
	//처음충돌체mesh와 이 엔티티의 메시가 자치하는 공간에대해서 일치시키여함으로
	DirectX::XMVECTOR center = DirectX::XMVectorDivide(DirectX::XMVectorAdd(pt1, pt2),DirectX::XMVectorSet(2.0f,2.0f,2.0f,1.0f));
	DirectX::XMVECTOR size = DirectX::XMVectorSubtract(pt2, pt1);	//,x,y,z,축에평행한모서리들의길이

	//기본 정육면체 메시의 사이즈는 중심은 원점이고, X Y ,Z축에 평행한 모서리들의 길이는 1.0이다.
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(center);
	DirectX::XMMATRIX scalingMatrix = DirectX::XMMatrixScalingFromVector(size);

	DirectX::XMFLOAT4X4 defaultMatrix;
	DirectX::XMStoreFloat4x4(&defaultMatrix, DirectX::XMMatrixMultiply(scalingMatrix, translationMatrix));

	collider->SetDefaultMatrix(defaultMatrix);
	Mesh* cubeMesh = MeshManager::GetMesh("Cube");

	collider->Initialize(boxDx, cubeMesh);

	return collider;
}

void Quad::ColliderGenerator::UploadFrustumVertexVector(FrustumCamera* camera, UploadBuffer* vertexUploadBuffer)
{

	float nearZ = camera->GetNearPlane();
	float farZ = camera->GetFarPlane();
	float aspect = camera->GetAspect();
	float tanFovY = tan(camera->GetFovY() / 2.0f);

	float nearTopY = tanFovY * nearZ;
	float nearBottomY = -nearTopY; // tan(camera->GetFovY())* nearZ;

	float nearRightX = tanFovY * aspect * nearZ;
	float nearLeftX = -1.0f * nearRightX;


	float farTopY = tanFovY * farZ;
	float farBottomY = -farTopY;

	float farRightX = tanFovY * aspect * farZ;
	float farLeftX = -1.0f * farRightX;


	std::vector<StaticVertex> mVertexVector(8);
	
	mVertexVector[0].mPos = { nearLeftX,nearBottomY,nearZ };
	mVertexVector[1].mPos = { nearLeftX,nearTopY,nearZ };
	mVertexVector[2].mPos = { nearRightX,nearBottomY,nearZ };
	mVertexVector[3].mPos = { nearRightX,nearTopY,nearZ };

	mVertexVector[4].mPos = { farLeftX,farBottomY,farZ };
	mVertexVector[5].mPos = { farLeftX,farTopY,farZ };
	mVertexVector[6].mPos = { farRightX,farBottomY,farZ };
	mVertexVector[7].mPos = { farRightX,farTopY,farZ };


	for (int i = 0; i < 8; ++i)
	{
		vertexUploadBuffer->CopyData(i, &mVertexVector[i], sizeof(StaticVertex));
	}



}

void Quad::ColliderGenerator::ReleaseCollider(Collider* collider)
{
	if (collider == nullptr)
		return;

	//위험
	BinnedAllocator * allocator =  GetColliderBinnedAllocator(collider->GetClassSize());
	allocator->Release(collider);

}

void Quad::ColliderGenerator::ReleaseCollider(UiCollider* collider)
{
	auto instance = GetInstance();
	switch (collider->GetColliderType())
	{
	case EUiColliderType::eRect:
	{
		instance->mRectColliderAllocator.ReleaseInstance(static_cast<RectCollider*>(collider));

	}
	}


}

Quad::Collider * Quad::ColliderGenerator::CreateDefaultCollider(const std::string& colliderClassName)
{

	auto instance = GetInstance();


	std::unordered_map<std::string, std::function<Collider*()>>::iterator it =  instance->mColliderDefaultConstructorTable.find(colliderClassName);
	Collider* collider = it->second();
	return collider;

}

Quad::RectCollider* Quad::ColliderGenerator::CreateRectCollider()
{
	auto instance = GetInstance();
	RectCollider* rectCollider = instance->mRectColliderAllocator.GetInstance();
	return rectCollider;
}

void Quad::ColliderGenerator::ReleaseRectCollider(RectCollider* rectCollider)
{
	if (rectCollider == nullptr)
		return;

	auto instance = GetInstance();
	instance->mRectColliderAllocator.ReleaseInstance(rectCollider);
	return;

}

Quad::SplineCollider* Quad::ColliderGenerator::CreateSplineCollider()
{


	auto instance = GetInstance();
	SplineCollider* splineCollider = instance->mSplineColliderAllocator.GetInstance();
	return splineCollider;

}

void Quad::ColliderGenerator::ReleaseSplineCollider(Quad::SplineCollider* splineCollider)
{
	if (splineCollider == nullptr)
		return;

	auto instance = GetInstance();
	instance->mSplineColliderAllocator.ReleaseInstance(splineCollider);
	return;


}



void Quad::ColliderGenerator::InitBoxIndexBuffer()
{
	
	std::vector<MeshIndexType> indexVector(36);
	//Near plane
	indexVector[0] = 0;
	indexVector[1] = 1;
	indexVector[2] = 2;

	indexVector[3] = 1;
	indexVector[4] = 3;
	indexVector[5] = 2;

	//far plane
	indexVector[6] = 4;
	indexVector[7] = 5;
	indexVector[8] = 6;

	indexVector[9] = 5;
	indexVector[10] = 7;
	indexVector[11] = 6;


	//left
	indexVector[12] = 0;
	indexVector[13] = 4;
	indexVector[14] = 2;

	indexVector[15] = 2;
	indexVector[16] = 4;
	indexVector[17] = 6;


	//right
	indexVector[18] = 2;
	indexVector[19] = 6;
	indexVector[20] = 3;

	indexVector[21] = 3;
	indexVector[22] = 6;
	indexVector[23] = 7;


	//top
	indexVector[24] = 0;
	indexVector[25] = 1;
	indexVector[26] = 4;

	indexVector[27] = 1;
	indexVector[28] = 5;
	indexVector[29] = 4;


	//bottom
	indexVector[30] = 1;
	indexVector[31] = 3;
	indexVector[32] = 5;

	indexVector[33] = 5;
	indexVector[34] = 3;
	indexVector[35] = 7;


	if (mGraphicsCommandObject->GetCloseState())
		mGraphicsCommandObject->ResetCommandList(nullptr);


	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer=nullptr;
	 mBoxIndexBuffer = Utility::CreateDefaultBuffer(mDevice, mGraphicsCommandObject->GetGraphicsCommandList(), indexVector.data(), sizeof(MeshIndexType),
		indexVector.size(), uploadBuffer);

	mGraphicsCommandObject->ExecuteCommandList();
	mGraphicsCommandObject->FlushCommandQueue();

	

}

void Quad::ColliderGenerator::InitFrustumIndexBuffer()
{

	std::vector<MeshIndexType> indexVector(36);

	//near plane
	indexVector[0] = 0;
	indexVector[1] = 1;
	indexVector[2] = 2;
	indexVector[3] = 1;
	indexVector[4] = 3;
	indexVector[5] = 2;

	//far plane 
	indexVector[6] = 4;
	indexVector[7] = 6;
	indexVector[8] = 5;
	indexVector[9] = 5;
	indexVector[10] =6;
	indexVector[11] = 7;


	//left
	indexVector[12] = 0;
	indexVector[13] = 4;
	indexVector[14] = 5;
	indexVector[15] = 0;
	indexVector[16] = 5;
	indexVector[17] = 1;


	//right
	indexVector[18] = 2;
	indexVector[19] = 7;
	indexVector[20] = 6;
	indexVector[21] = 2;
	indexVector[22] = 3;
	indexVector[23] = 7;


	//top
	indexVector[24] = 1;
	indexVector[25] = 5;
	indexVector[26] = 7;
	indexVector[27] = 1;
	indexVector[28] = 7;
	indexVector[29] = 3;


	//bottom
	indexVector[30] = 4;
	indexVector[31] = 6;
	indexVector[32] = 5;
	indexVector[33] = 5;
	indexVector[34] = 6;
	indexVector[35] = 7;

	if (mGraphicsCommandObject->GetCloseState())
		mGraphicsCommandObject->ResetCommandList(nullptr);

	Microsoft::WRL::ComPtr<ID3D12Resource> tempUploadBuffer;
	mFrustumIndexBuffer  = Utility::CreateDefaultBuffer(mDevice, mGraphicsCommandObject->GetGraphicsCommandList(), indexVector.data(), sizeof(MeshIndexType), indexVector.size(), tempUploadBuffer);
	
	mGraphicsCommandObject->ExecuteCommandList();
	mGraphicsCommandObject->FlushCommandQueue();

}

void Quad::ColliderGenerator::InitColliderDefaultConstructor()
{

	mColliderDefaultConstructorTable["BoxCollider"] = []()->Collider* {	auto* allocator = GetColliderBinnedAllocator(sizeof(BoxCollider));
	BoxCollider* collider = new	(allocator->Alloc())BoxCollider;



		//collider->SetMesh(MeshManager::GetMesh("Cube"));












	return collider;};
	 


	mColliderDefaultConstructorTable["FrustumCollider"] = []()->Collider* {	auto* allocator = GetColliderBinnedAllocator(sizeof(FrustumCollider));
	FrustumCollider* collider = new	(allocator->Alloc())FrustumCollider;
	return collider; };


	mColliderDefaultConstructorTable["OrientedBoxCollider"] = []()->Collider* {	auto* allocator = GetColliderBinnedAllocator(sizeof(OrientedBoxCollider));
	OrientedBoxCollider* collider = new	(allocator->Alloc())OrientedBoxCollider;
	return collider; };


	mColliderDefaultConstructorTable["SphereCollider"] = []()->Collider* {	auto* allocator = GetColliderBinnedAllocator(sizeof(SphereCollider));
	SphereCollider* collider = new	(allocator->Alloc())SphereCollider;
	return collider; };




	mColliderDefaultConstructorTable["CameraBoxCollider"] = []()->Collider* {	auto* allocator = GetColliderBinnedAllocator(sizeof(CameraBoxCollider));
	CameraBoxCollider* collider = new	(allocator->Alloc())CameraBoxCollider;
	return collider; };




	mColliderDefaultConstructorTable["CameraFrustumCollider"] = []()->Collider* {	auto* allocator = GetColliderBinnedAllocator(sizeof(CameraFrustumCollider));
	CameraFrustumCollider* collider = new	(allocator->Alloc())CameraFrustumCollider;
	return collider; };

}

Microsoft::WRL::ComPtr<ID3D12Resource> Quad::ColliderGenerator::GetFrustumIndexBuffer() const
{
	return mFrustumIndexBuffer;
}
