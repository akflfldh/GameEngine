#pragma once

#include"Predeclare.h"
#include"Asset/Mesh/Mesh.h"
#include"Core/CoreDllExport.h"

namespace Quad
{
	struct CORE_API_LIB InstancingData
	{
		Mesh* mOriginalMesh;
		std::vector<SubMesh> mSubMesh;
	};

}
