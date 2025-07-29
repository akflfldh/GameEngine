#pragma once

#include"RenderType.h"
#include"MaterialType.h"
#include"RenderSystemDllMacro.h"
namespace Render
{

	class RENDER_SYSTEM_API IMaterialManager
	{
	public:
		static IMaterialManager* GetInstance();
		IMaterialManager();
		virtual ~IMaterialManager() = 0;


		virtual Render::MaterialID CreateMaterial(const Render::CreationMaterialInfo& creationMaterialInfo) =0 ;


	//	bool GetMaterialItem(Render::MaterialID materialID, D3DMaterialItem& oMaterialItem) const;

		static void SetMaterialManagerImpl(IMaterialManager* pImpl);

	protected:

	private:
		 static IMaterialManager* mImpl;

	};

}
