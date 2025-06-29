#pragma once

#include"Predeclare.h"
#include"Object/Object.h"
#include"Component/Model.h"
#include"DirectXCollision.h"

#include"Core/Transform.h"

#include"BaseCollider.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	enum class EColliderType :int
	{
		eBox=0,
		eSphere,
		eOrientedBox,
		eFrustum
	};

	class CORE_API_LIB Collider:public BaseCollider
	{
		friend class CollisionHelper;
		friend class ColliderGenerator;

	public:
		Collider(EColliderType colliderType);
		virtual ~Collider() =0;

		virtual void Initialize();
		virtual void Update(const DirectX::XMFLOAT4X4& transformMatrix)=0;//직접지정한다.
		virtual void Update(const DirectX::XMMATRIX & transformMatrix)=0;//직접지정한다.

		CLASSNAME(Collider)
		CLASSSIZE(Collider)

		EColliderType GetColliderType() const;

		virtual void SetSystem(System* system) ;
		virtual System* GetSystem() const ;

		virtual void Serialize();
		virtual void DeSerialize();

		virtual void SetPositionLocal(const DirectX::XMFLOAT3& pos) = 0;

		void SetMap(Map* map);
		virtual  Map* GetMap() const ;
		virtual Map* GetMap()  ;

	protected:

	private:

		EColliderType mColliderType;
		System* mSystem;

		Map* mMap;
		EObjectType mType;

	};









}
