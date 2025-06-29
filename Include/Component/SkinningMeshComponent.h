#pragma once


#include"Component/IMeshComponent.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB SkinningMeshComponent:public IMeshComponent
	{
	public:
		SkinningMeshComponent();
		virtual ~SkinningMeshComponent();

		virtual void Start() override;
		virtual void Update(float deltaTime) override;


		virtual void SetMesh(Mesh* mesh) override;

		virtual void Serialize(const std::string& tag = "") override;
		virtual void DeSerialize(const std::string& tag = "") override;

		CLASSSIZE(SkinningMeshComponent)
			CLASSNAME(SkinningMeshComponent)
			CLASSNAMESTATIC(SkinningMeshComponent)

	private:
		void SetMeshToCollider(Mesh* mesh);


	};
	REGISTERCOMPONENTCLASS(SkinningMeshComponent)
}

