#pragma once


#include"Predeclare.h"
#include"Utility/Singleton.h"
#include"header.h"
#include<unordered_map>
#include"Core/CoreDllExport.h"
#include"Allocator/DestructingPoolAllocator.h"
#include"Collision/RectCollider.h"
#include "Collision/SplineCollider.h"
namespace Quad
{
	class CORE_API_LIB ColliderGenerator :public Singleton<ColliderGenerator>
	{
	public:
		ColliderGenerator();
		~ColliderGenerator() = default;

		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, GraphicCommand* graphicsCommandObject);

		static Collider* CreateBoxCollider(Mesh* mesh);
		static Collider* CreateBoxCollider(const DirectX::XMFLOAT3& minPoint, const DirectX::XMFLOAT3 & maxPoint);
		static Collider* CreateFrustumCollider(FrustumCamera* caerma);
		static CameraFrustumCollider* CreateCameraFrustumCollider(FrustumCamera* camera);

		static CameraBoxCollider* CreateBoxCameraCollider(OrthogoanlCamera* camera);


		static void UploadFrustumVertexVector(FrustumCamera* camera,UploadBuffer * vertexUploadBuffer);

		static void ReleaseCollider(Collider* collider);
		static void ReleaseCollider(UiCollider* collider);


		static Collider * CreateDefaultCollider(const std::string& colliderClassName);


		static RectCollider* CreateRectCollider();
		static void ReleaseRectCollider(RectCollider* rectCollider);
		
		static SplineCollider* CreateSplineCollider();
		static void ReleaseSplineCollider(SplineCollider* splineCollider);

		

	private:
		void InitBoxIndexBuffer();
		void InitFrustumIndexBuffer();

		void InitColliderDefaultConstructor();

		Microsoft::WRL::ComPtr<ID3D12Resource> GetFrustumIndexBuffer() const;


	private:
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		GraphicCommand* mGraphicsCommandObject;
		Microsoft::WRL::ComPtr<ID3D12Resource> mBoxIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> mFrustumIndexBuffer;


		std::unordered_map<std::string, unsigned int> mColliderSizeTable;

		std::unordered_map<std::string, std::function<Collider* ()>> mColliderDefaultConstructorTable;



		DestructingPoolAllocator< RectCollider> mRectColliderAllocator;
		DestructingPoolAllocator< SplineCollider> mSplineColliderAllocator;


	};

}
