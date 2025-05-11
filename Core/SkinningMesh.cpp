#include "Asset/Mesh/SkinningMesh.h"

Quad::SkinningMesh::SkinningMesh(const std::string& name)
	:Mesh(name,EMeshType::eSknningMesh)
{
}



Quad::SkinningMesh::~SkinningMesh()
{
}

void Quad::SkinningMesh::SetVertexVector(std::vector<SkinningVertex>&& vertexVector)
{
	mVertexVector = std::move(vertexVector);
	SetVertexNum(mVertexVector.size());

}

const std::vector<Quad::SkinningVertex> Quad::SkinningMesh::GetVertexVector() const
{
	return mVertexVector;
}



D3D12_VERTEX_BUFFER_VIEW Quad::SkinningMesh::GetVertexBufferView()
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = GetVertexBuffer()->GetGPUVirtualAddress();
	vbv.StrideInBytes = sizeof(mVertexVector[0]);
	vbv.SizeInBytes = vbv.StrideInBytes * GetVertexNum();

	return vbv;
}

bool Quad::SkinningMesh::CheckType(const ContentItem* source)
{
	bool ret =	Mesh::CheckType(source);
	if (!ret)
		return false;

	const Mesh* mesh = static_cast<const Mesh*>(source);

	if (mesh->GetMeshType() != Quad::EMeshType::eSknningMesh)
		return false;

	return true;
}


bool Quad::SkinningMesh::InnerCopy(const ContentItem* source)
{

	Mesh::InnerCopy(source);

	const SkinningMesh* skinningMesh = static_cast<const SkinningMesh*>(source);
	mVertexVector = skinningMesh->mVertexVector;

	return true;

}