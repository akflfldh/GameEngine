#include "Asset/Mesh/StaticMesh.h"

//void Quad::StaticMesh::SetVertexVector(std::vector<StaticVertex>&& vertexVector)
//{
//	mVertexVector = vertexVector;
//}

//const std::vector<Quad::StaticVertex> Quad::StaticMesh::GetVertexVector() const
//{
//	return mVertexVector;
//}

Quad::StaticMesh::StaticMesh(const std::string& name)
	:Mesh(name,EMeshType::eStaticMesh)
{
}

Quad::StaticMesh::~StaticMesh()
{
}

void Quad::StaticMesh::SetVertexVector(std::vector<StaticVertex>&& vertexVector)
{
	mVertexVector = std::move(vertexVector);
	SetVertexNum(mVertexVector.size());
}



const std::vector<Quad::StaticVertex>& Quad::StaticMesh::GetVertexVector() const
{
	return mVertexVector;
	// TODO: 여기에 return 문을 삽입합니다.
}


D3D12_VERTEX_BUFFER_VIEW Quad::StaticMesh::GetVertexBufferView()
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = GetVertexBuffer()->GetGPUVirtualAddress();
	vbv.StrideInBytes = sizeof(mVertexVector[0]);
	vbv.SizeInBytes = vbv.StrideInBytes * GetVertexNum();

	return vbv;
}

bool Quad::StaticMesh::CheckType(const Quad::ContentItem* source)
{
	bool ret =	Mesh::CheckType(source);
	if (!ret)
		return false;

	const Mesh* mesh = static_cast<const Mesh*>(source);

	if (mesh->GetMeshType() != Quad::EMeshType::eStaticMesh)
		return false;



	return true;
}

bool Quad::StaticMesh::InnerCopy(const ContentItem* source)
{
	Mesh::InnerCopy(source);


	const StaticMesh* staticMesh = static_cast<const StaticMesh*>(source);
	mVertexVector = staticMesh->mVertexVector;


	return true;
}
