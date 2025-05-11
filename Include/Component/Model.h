#pragma once


#include"Predeclare.h"
#include"Asset/Mesh/Mesh.h"
#include"AnimationComponent.h"
#include"ResourceSmartPointer/MeshSmartPointer.h"
#include"Component.h"
#include"Allocator/BinnedAllocator.h"

#include"CoreDllExport.h"

namespace Quad
{

	


	class CORE_API_LIB Model
	{
		friend class RenderSystem;

	public:
		Model(SceneElement* destSceneElement);
		~Model();

		

		void Initialize();

		void Start();
		void Update(float deltaTime);

		



	
		bool GetDirtyFlag() const;


		void Serialize(const std::string & tag);
		void DeSerialize(const std::string & tag);


		void AddComponernt(EComponentType componentType);
		void RemoveComponent(EComponentType componentType);

		ModelBaseComponent* GetComponent(EComponentType componentType) const;

		MeshComponent* GetMeshComponent() const ;
		AnimationComponent* GetAnimationComponent() const ;
		ColliderComponent* GetColliderComponent() const;
		UiColliderComponent* GetUiColliderComponent()const; 

		MeshComponent* GetMeshComponent() ;
		AnimationComponent* GetAnimationComponent() ;
		ColliderComponent* GetColliderComponent() ;
		

	private:
		//std::string mName;
		//model이 mesh에서 공통인부분(바뀌지않는부분을위해서 원본을 참조하고 , model 별로 바뀔수있는부분은 (submesh)따로 가져야한다.
		//원본은 컨테이너에 그대로 있는거다.

		//submesh의 material부분이 바뀔수있다 현재는 texture만 바꾸는게 가능하다.
		//flag 를 써서 개별적으로 바꾸기전까지는 원본이 바뀌면 같이 바뀌도록할수있다.간단히,다만 그것은 구현은 지금 안함.

		//model은 씬내의 엔티티가 가지는 기하형상,재질등을 추상화한 타입이라고할수있겠다.

		//Mesh* mMesh;
		//MeshSmartPointer mMeshPointer;
		
		//Collider* mCollider;
		//std::vector<SubMesh> mSubMeshVector;
		//std::vector<ModelSubMesh> mSubMeshVector;
		//AnimationComponent mAnimationComponent;




		bool mMeshDirtyFlag;
		bool mAnimationClipDirtyFlag;
		bool mSkeletonDirtyFlag;


		SceneElement* mDestSceneElement;

		std::vector<ModelBaseComponent*> mModelComponentVector;

	};


}