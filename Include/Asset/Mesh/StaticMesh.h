#pragma once



#include"Predeclare.h"
#include"Asset/Mesh/Mesh.h"

#include"Core/CoreDllExport.h"

namespace Quad

{
	class CORE_API_LIB StaticMesh:public Mesh
	{
	public:
		StaticMesh(const std::string& name = "");
		~StaticMesh();

		void SetVertexVector(std::vector<StaticVertex>&& vertexVector);
		const std::vector<StaticVertex>  & GetVertexVector() const;


		virtual D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() override;


		virtual bool CheckType(const Quad::ContentItem* source) override;
	protected:
		virtual bool InnerCopy(const ContentItem* source)override;



	private:
		std::vector<StaticVertex> mVertexVector;



	};

}

