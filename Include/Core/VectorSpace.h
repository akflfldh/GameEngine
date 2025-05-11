#pragma once

#include"Predeclare.h"
#include"Core/SpacePartitioningStructure.h"
#include"Object/Macro.h"

#include"CoreDllExport.h"
namespace Quad
{
	template<typename collider>
	class  VectorSpace :public SpacePartitioningStructure<collider>
	{
	public:
		VectorSpace() = default;
		~VectorSpace();


		void Initialize(size_t objectMaxNum);

		virtual bool Copy(SpacePartitioningStructure<collider>* source) override;


		CLASSNAME(VectorSpace)
		
		
		virtual void AddCollider(collider* collider)override;
		virtual void RemoveCollider(collider* collider) override;
		virtual void DeleteCollider(collider* collider)override;

		virtual void ReBuild()override;

		virtual void FrsutumCullingCollider(std::vector<collider*>& oColliderVector, Camera* camera)override;
		virtual bool RayCasting(std::vector<collider*>& oColliderVector, const Ray& ray) override;
		virtual bool RayCastingFirst(collider*& oCollider,  Ray& ray) override;
		virtual void GetAllCollider(std::vector<collider*>& oColliderVector)override;



		void Reset() override;
	private:
		std::vector<collider*> mColliderVector;
		size_t mColliderMaxNum;

	};



	//Collider, UiCollider 특수화
















}