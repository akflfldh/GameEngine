#pragma once


#include"Predeclare.h"
#include"header.h"
#include"Buffer/UploadBuffer.h"
#include"Object/Object.h"
#include"Asset/Material/LineBaseMaterial.h"
#include"Asset/Mesh/MeshType.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	struct LineVertex
	{
		DirectX::XMFLOAT3 mPos;
	};

	class CORE_API_LIB LineBase:public Object
	{
		friend class LineFactory;
	public:
		LineBase(const std::string& name = "", EObjectType objectType =EObjectType::eLine);
		virtual ~LineBase() = 0;


		CLASSNAME(LineBase)


		void SetVertexBuffer(UploadBuffer* vertexBuffer);

		D3D12_GPU_VIRTUAL_ADDRESS GetVertexBufferGpuVirtualAddress() const;
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

		/*void SetVertexVector(std::vector<LineVertex>&& vertexVector);
		void SetVertexVector(const std::vector<LineVertex>& vertexVector);
		const std::vector<LineVertex>& GetVertexVector() const;*/

		void SetEffectName(const std::string& name);
		const std::string& GetEffectName()const;

		size_t GetVertexNum() const;
		void SetVertexNum(size_t num);


	//	const LineBaseMaterial& GetMaterial() const ;
	//	LineBaseMaterial& GetMaterial();


		const LineModelSubMesh* GetModelSubMesh() const;
		LineModelSubMesh* GetModelSubMesh();

		//이 메서드는 아무것도하지않고 인터페이스역할만 한다.
		virtual bool InnerDetectCollsion(Collider* colliderA, Collider* colliderB) override;
		virtual bool InnerDetectRayHit(const Ray& ray) override;


	protected:
		UploadBuffer* GetVertexBuffer() const;

	private:

		UploadBuffer* mUploadVertexBuffer;
		size_t mVertexNum;
		std::string mEffectName;
		//LineBaseMaterial mMaterial;
		LineModelSubMesh mModelSubMesh;

	};

}
