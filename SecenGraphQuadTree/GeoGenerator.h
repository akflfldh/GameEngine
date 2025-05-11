#pragma once

#include"RawResourceType.h"
namespace Quad
{
	class GeometryGenerator
	{
	public:

		GeometryGenerator() = default;
		~GeometryGenerator() = default;


		static bool CreateRawRectMeshData(RawStaticMeshData *  oRawMeshData);
		static bool CreateRawBoundaryRectMeshData(RawStaticMeshData* oRawMeshData);

	private:



	};


	
}