#pragma once


#include"Component/ModelBaseComponent.h"

#include"Core/CoreDllExport.h"

namespace Quad
{
	class Collider;
	class CORE_API_LIB ColliderComponent:public ModelBaseComponent
	{

	public:
		ColliderComponent(SceneElement * sceneElement);
		~ColliderComponent();

		
		virtual void Update(float deltaTime) override;

		//기존 콜라이더는 맵에서 제거+완전히 삭제
		void SetCollider(Collider* collider);

		//collider 타입은 그대로유지
		void SetMesh(Mesh* mesh);

		virtual void Serialize(const std::string& tag) override;
		virtual void DeSerialize(const std::string& tag) override;
		CLASSSIZE(ColliderComponent)

		Collider* GetCollider();
		const Collider* GetCollider() const;

	private:
		Collider* mCollider;

	};



}

