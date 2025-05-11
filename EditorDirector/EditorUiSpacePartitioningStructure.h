#pragma once

#include"SpacePartitioningStructure.h"

namespace Quad
{
	class EditorUiSpacePartitioningStructure:public SpacePartitioningStructure<UiCollider>
	{
	public:
		EditorUiSpacePartitioningStructure(SpacePartitioningStructure* implSpaceStructure);
		~EditorUiSpacePartitioningStructure() = default;

		virtual const char* GetClassTypeName() const override;

		virtual void AddCollider(UiCollider* collider) override;
		virtual void RemoveCollider(UiCollider* collider) override;
		virtual void DeleteCollider(UiCollider* collider) override;

		virtual void ReBuild()override;

		virtual void FrsutumCullingCollider(std::vector<UiCollider*>& oColliderVector, Camera* camera)override;
		virtual bool RayCasting(std::vector<UiCollider*>& oColliderVector, const Ray& ray)override;
		virtual bool RayCastingFirst(UiCollider*& oCollider, Ray& ray) override;
		virtual void GetAllCollider(std::vector<UiCollider*>& oColliderVector) override;



		bool RayCastingFirstNotIncludeEditorObject(UiCollider*& oCollider, Ray& ray) const;
		void AddGizmoCollider(UiCollider* collider);
	//	const std::vector<UiCollider*>& GetGizmoColliderVector() const;


		//콜라이더를 컨테이너에서 모두 없애야한다.(일단 기즈모는 무시해보자)
		virtual void Reset() override;


		SpacePartitioningStructure<UiCollider>* GetImplSpaceStructure();
	private:

		SpacePartitioningStructure< UiCollider>* mImplSpaceStructure;
		std::vector<UiCollider* > mGizmoColliderVector;



	};

}
