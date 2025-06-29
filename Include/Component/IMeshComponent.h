#pragma once

#include"Component/SceneComponent.h"
#include"Asset/Mesh/MeshType.h"
#include"Collision/BaseCollider.h"
#include"ResourceSmartPointer/MeshSmartPointer.h"
#include"Core/CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB IMeshComponent:public SceneComponent
	{
	public:
		IMeshComponent();
		virtual ~IMeshComponent() = 0;

		virtual void SetMesh(Mesh* mesh);
		Mesh* GetMesh() const;

		virtual void SetActiveState(bool activeState) override;

		const std::vector<ModelSubMesh>& GetSubMesh() const;
		std::vector<ModelSubMesh>& GetSubMesh();

		void SetMaterial(size_t subMeshIndex, Material* material);
		
		void SetTextureToAllSubMesh(Texture* texture);


		virtual void OnActive() override;
		virtual void DeActive() override;


		virtual void Serialize(const std::string& tag = "") override;
		virtual void DeSerialize(const std::string& tag = "") override;

		virtual BaseCollider* GetCollider() const;

	protected:
		void SetCollider(BaseCollider* collider);

		MeshSmartPointer mMeshPointer;
		std::vector<ModelSubMesh> mSubMeshVector;
		BaseCollider* mPickingCollider;
	private:





	};

}
