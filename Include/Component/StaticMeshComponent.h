#pragma once

#include"Predeclare.h"
#include"ResourceSmartPointer/MeshSmartPointer.h"
#include"Asset/Mesh/MeshType.h"
#include<vector>



#include"Component/IMeshComponent.h"


#include"CoreDllExport.h"
#include"Collision/BoxCollider.h"



namespace Quad
{
	//3d mesh 
	class CORE_API_LIB StaticMeshComponent:public IMeshComponent
	{
	public:
		StaticMeshComponent();
		virtual ~StaticMeshComponent();
		
		virtual void Start() override;
		virtual void Update(float deltaTime) override;


		virtual void SetMesh(Mesh* mesh) override;

		virtual void Serialize(const std::string& tag="") override;
		virtual void DeSerialize(const std::string& tag="") override;

		CLASSSIZE(StaticMeshComponent)
		CLASSNAME(StaticMeshComponent)
		CLASSNAMESTATIC(StaticMeshComponent)

	private:
		void SetMeshToCollider(Mesh * mesh);

	};
	REGISTERCOMPONENTCLASS(StaticMeshComponent)

}
