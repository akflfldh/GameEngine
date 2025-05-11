#pragma once

#include"Predeclare.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB ComponentIDGenerator
	{
	public:
		ComponentIDGenerator() = default;
		~ComponentIDGenerator() = default;


		template<typename ComponentType>
		static int GetID();

	private:
		inline static int mComponentID =0; 

	};

	template<typename ComponentType>
	inline int ComponentIDGenerator::GetID()
	{
		static int id = mComponentID++;
		return id;
	}

}
