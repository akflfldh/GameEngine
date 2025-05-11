#pragma once

#include"Predeclare.h"
#include"Object/Object.h"
#include"Component/Model.h"
#include"DirectXCollision.h"

#include"Core/Transform.h"
#include"Core/SceneElement.h"
#include"Component/MeshComponent.h"

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

	class CORE_API_LIB Collider:public SceneElement
	{
		friend class CollisionHelper;
		friend class ColliderGenerator;

	public:
		Collider(EColliderType colliderType);
		virtual ~Collider() =0;
		virtual void OnEvent(Event* event) =0;
		virtual void Update(float deltaTime)= 0;//기본적으로 destObject의 worldMatrix을 이용해서 변환한다.
		virtual void Update(float deltaTime, const DirectX::XMFLOAT4X4& transformMatrix)=0;//직접지정한다.
		virtual void Update(float deltaTime, const DirectX::XMMATRIX & transformMatrix)=0;//직접지정한다.

		CLASSNAME(Collider)
		CLASSSIZE(Collider)
		//엔티티의 메시가 바뀔수도있는경우도있다.!!!!!!!!!!!!!!!!!!!!!!!

		//자신이속한 엔티티를 알고있고 그 엔티티의 transform접근해서 변환정보를 얻어와서
		//월드공간으로 업데이트하자.
		//void SetEntity();

		//메시를 넘겨서 초기화할때 그순간에바로 충돌체도 처리를해줘야한다
		//그다음부터 변환하면 그 변환에따라 같이 업데이트된다.
		
		//const Model& GetModel()const ;
		//Model& GetModel() ;

		Object* GetDestObject() const;


		void SetDestObject(Object* object);
		EColliderType GetColliderType() const;

		virtual void SetSystem(System* system) override;
		virtual System* GetSystem() const override;

		virtual void Serialize() = 0 ;
		virtual void DeSerialize()=0 ;


		void SetMap(Map* map);
		virtual  Map* GetMap() const override;
		virtual Map* GetMap()  override;

		Transform& GetTransform();
		const Transform& GetTransform() const;


		virtual void SetMesh(Mesh * mesh)= 0;
	

		MeshComponent* GetMeshComponent();
		const MeshComponent* GetMeshComponent()const;
	
	protected:
		
		void Initialize(Mesh* mesh);
		void SetDefaultMatrix(const DirectX::XMFLOAT4X4& defaultMatrix);
		const DirectX::XMFLOAT4X4 & GetDefaultMatrix() const;

	private:

		Object* mObject;
		Transform mTransform;
		EColliderType mColliderType;

	//	Model mModel;
		MeshComponent * mMeshComponent;
		System* mSystem;

		Map* mMap;
		DirectX::XMFLOAT4X4 mDefaultMatrix;//기본적으로 충돌체메시를 엔티티의 메시가자치하는영역에 맞도록 조정하는 
		EObjectType mType;



	};









}
