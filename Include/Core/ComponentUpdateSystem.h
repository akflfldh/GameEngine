#pragma once

#include"Predeclare.h"
#include<vector>
#include<unordered_map>
#include"Core/CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB ComponentUpdateSystem
	{
	public:
		ComponentUpdateSystem() = default;
		virtual ~ComponentUpdateSystem() = 0;



		virtual void AddObject(Object* object);
		virtual void RemoveObject(Object* object);

		virtual void Reset();

	protected:
		
		std::vector<Object*>& GetObjectVector();
		const std::vector<Object*>& GetObjectVector()const;



	private:

		std::vector<Object*> mObjectVector;
						//<id , index>
		std::unordered_map<unsigned long long, size_t> mIndexTable;


	};

}
