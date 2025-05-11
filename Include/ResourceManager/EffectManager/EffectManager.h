#pragma once

#include"Utility/Singleton.h"
#include"Utility/UniqueIDTable.h"
#include"Utility/NameTable.h"
#include"Allocator/DestructingPoolAllocator.h"
#include"Effect/Effect.h"


#include"CoreDllExport.h"
namespace Quad
{
	class Effect;

	class CORE_API_LIB EffectManager:public Singleton<EffectManager>
	{
		friend class ResourceStorer;
	public:
		EffectManager();
		~EffectManager();
		

		static Effect* CreateEffect(const std::string& name);

		static Effect* GetEffect(unsigned long long id);
		static Effect* GetEffect(const std::string& name);

		static void RemoveEffect(unsigned long long id);
		static void RemoveEffect(const std::string & name);



	private:
		DestructingPoolAllocator<Effect> mPoolAllocator;
		UniqueIDTable<Effect> mIDTable;
		NameTable mNameTable;



	};

}
