#pragma once

#include"header.h"
#include"Asset/Asset.h"
#include"Asset/Material/Material.h"
#include"Asset/Mesh/MeshType.h"

//#include"MeshManager.h"

#include"Core/CoreDllExport.h"
namespace Quad
{


	class CORE_API_LIB Mesh:public Asset
	{
		friend class MeshManager;
	public:
		
		Mesh(const std::string& name ="", EMeshType meshTyp = EMeshType::eStaticMesh);
		virtual ~Mesh()=0 ;

		void SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer);
		void SetIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer);
		void SetIndexVector(std::vector<MeshIndexType>&& indexVector);
		void SetSubMeshVector(std::vector<SubMesh>&& subMeshVector);
		
		void SetVertexUploadBuffer(UploadBuffer* uploadBuffer);
		UploadBuffer* GetVertexUploadBuffer() const;

		const std::vector<SubMesh>& GetSubMeshVector() const;
		const std::vector<MeshIndexType> GetIndexVector() const;
		D3D12_GPU_VIRTUAL_ADDRESS GetVertexBufferGpuVirtualAddress();
		D3D12_GPU_VIRTUAL_ADDRESS GetIndexBufferGpuVirtualAddress();

		virtual D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() = 0;
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView()const;

		void SetMeshType(EMeshType meshType);
		EMeshType GetMeshType() const;
		void SetMinMaxPoint(const DirectX::XMFLOAT3& minPoint, const DirectX::XMFLOAT3& maxPoint);
		std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> GetMinMaxPoint()const;


		void SetIndexNum(UINT num);
		void SetVertexNum(UINT num);

		UINT GetIndexNum()const;
		UINT GetVertexNum() const;

		virtual void Serialize(const std::string& tag = "") override;
		virtual void DeSerialize(const std::string& tag = "") override;


		virtual bool CheckType(const ContentItem* source) override;

	protected:

		Microsoft::WRL::ComPtr<ID3D12Resource> GetVertexBuffer() const;
		Microsoft::WRL::ComPtr<ID3D12Resource> GetIndexBuffer() const;


		virtual bool InnerCopy(const ContentItem* source) override;


	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> mIndexBuffer;
		
		UploadBuffer* mVertexUploadBuffer;//uploadBuffer를사용할경우 보관용


		std::vector<MeshIndexType> mIndexVector;

		std::vector<SubMesh> mSubMeshVector;

		UINT mIndexNum;
		UINT mVertexNum;


		DirectX::XMFLOAT3 mMinPoint;
		DirectX::XMFLOAT3 mMaxPoint;


		EMeshType mMeshType;


	
	};




	void CaculateStaticMeshNormalVector(std::vector<StaticVertex>& vertexVector, const std::vector<MeshIndexType>& indexVector);
	void CaculateSkinningMeshNormalVector(std::vector<SkinningVertex>& vertexVector, const std::vector<MeshIndexType>& indexVector);

}


//템플릿 특수화