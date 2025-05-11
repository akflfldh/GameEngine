#pragma once

#include"Predeclare.h"
#include"Asset/Mesh/Mesh.h"


#include"Core/CoreDllExport.h"

namespace Quad
{

	class CORE_API_LIB SkinningMesh:public Mesh
	{
	public:
		SkinningMesh(const std::string& name="");
		//SkinningMesh(const std::string& name = "");
		~SkinningMesh();


		void SetVertexVector(std::vector<SkinningVertex>&& vertexVector);
		const std::vector<SkinningVertex>  GetVertexVector() const;


		virtual D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView()override;

		virtual bool CheckType(const ContentItem* source) override;

	protected:

		virtual bool InnerCopy(const ContentItem* source) override;

	private:
		std::vector<SkinningVertex> mVertexVector;

	};


}

