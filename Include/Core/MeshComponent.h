#pragma once

#include"Predeclare.h"
#include"ResourceSmartPointer/MeshSmartPointer.h"
#include"Asset/Mesh/MeshType.h"
#include<vector>
#include"Component/ModelBaseComponent.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB MeshComponent:public ModelBaseComponent
	{
	public:
		MeshComponent(SceneElement * sceneElement);
		~MeshComponent();

		virtual void Update(float deltaTime) override;
		void SetMesh(Mesh* mesh); //메시가 유저에게 들어나는데 그러지말고 핸들을사용하고 	
		//외부로 들어날수있는 함수만 전역함수로 나타내고 그 전역함수안에서는 메서드를 호출해서 수행하는거지
		// 
		Mesh* GetMesh() const;

		EMeshType GetCurrentModelMeshType() const;


		//std::string GetName() const;
		const std::vector<ModelSubMesh>& GetSubMesh() const;
		std::vector<ModelSubMesh>& GetSubMesh();

		void SetMaterial(size_t subMeshIndex, Material* material);

		void SetTextureToAllSubMesh(Texture* texture);

		virtual void Serialize(const std::string& tag="") override;
		virtual void DeSerialize(const std::string& tag="") override;

		CLASSSIZE(MeshComponent)
	private:
		MeshSmartPointer mMeshPointer;
		std::vector<ModelSubMesh> mSubMeshVector;
	};

}
