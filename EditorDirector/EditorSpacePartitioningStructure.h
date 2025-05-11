#pragma once


#include"SpacePartitioningStructure.h"
namespace Quad
{
	
	class EditorSpacePartitioningStructure:public SpacePartitioningStructure<Collider>
	{
	public:
		EditorSpacePartitioningStructure(SpacePartitioningStructure * implSpaceStructure);
		~EditorSpacePartitioningStructure() = default;

		virtual const char* GetClassTypeName() const override;

		virtual void AddCollider(Collider* collider) override;
		virtual void RemoveCollider(Collider* collider) override;
		virtual void DeleteCollider(Collider* collider) override;

		virtual void ReBuild()override;

		virtual void FrsutumCullingCollider(std::vector<Collider*>& oColliderVector, Camera* caemra)override;
		virtual bool RayCasting(std::vector<Collider*>& oColliderVector, const Ray& ray)override;
		virtual bool RayCastingFirst(Collider*& oCollider, Ray& ray) override;
		virtual void GetAllCollider(std::vector<Collider*>& oColliderVector) override;



		bool RayCastingFirstNotIncludeEditorObject(Collider*& oCollider, Ray& ray) const ;
		void AddGizmoCollider(Collider* collider);
		const std::vector<Collider*>& GetGizmoColliderVector() const;


		//콜라이더를 컨테이너에서 모두 없애야한다.(일단 기즈모는 무시해보자)
		virtual void Reset() override;


		SpacePartitioningStructure<Collider>* GetImplSpaceStructure();
	private:

		SpacePartitioningStructure* mImplSpaceStructure;
		std::vector<Collider* > mGizmoColliderVector;


	};

}
