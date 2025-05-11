#pragma once

#include<vector>
#include"Asset/Mesh/MeshType.h"

namespace Quad
{
	class VertexHelper
	{

	public:
		static void CaculateSkinningMeshNormalVector(std::vector<SkinningVertex>& vertexVector, const std::vector<MeshIndexType>& indexVector);
		static void CaculateStaticMeshNormalVector(std::vector<StaticVertex>& vertexVector, const std::vector<MeshIndexType>& indexVector);
		


	};


}