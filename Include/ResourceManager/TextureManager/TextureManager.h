#pragma once


#include<unordered_map>

#include"Predeclare.h"
#include"Asset/Texture/Texture.h"
#include"Utility/Singleton.h"
#include"Utility/UniqueIDTable.h"
#include"Utility/NameTable.h"
#include"ResourceFactory/TextureFactory/TextureFactory.h"


#include"Core/CoreDllExport.h"


#define TEXTURE_MAXNUM 100000

#define EngineMode


namespace Quad
{
	class CORE_API_LIB TextureManager:public Singleton<TextureManager>
	{
		friend class ResourceStorer;
		friend class ResourceController;
#ifdef EngineMode
		friend class EngineModeDirector;
		friend class EditorAssetImportVersionModule;
#endif

#ifdef EditorMode
		friend class EditorModeDirector;
#endif


	public:


		TextureManager();
		~TextureManager() = default;


		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,DescriptorHeapManagerMaster* descriptorHeapManagerMaster,GraphicCommand * commandObject );




		static Texture* CreateTexture(const std::string& name);		//처음생성하면 Mesh에 아무것도 설정되지않을상태
		static Texture* CreateTextureFromFile(const std::string& name, unsigned long long id);

		static RenderTargetTexture* CreateRenderTargetTexture(const std::string& name, int width, int height);
		static Texture* CreateDepthStencilBuffer(const std::string& name, int width, int height);


		static Texture* GetTexture(const std::string& name);
		static Texture* GetTexture(unsigned long long id);

		//static bool  CheckDuplicate(const std::string& name);
		
		//static bool AddTexture(Texture* texture, const std::string& name);


		static void KillTexture(Texture* texture);
		static void KillTexture(const std::string & name);
		static void RemoveDeadTexture();


	private:
		TextureFactory mTextureFactory;
		//DeadTable
		std::vector<Texture*> mDeadTable;
		UniqueIDTable<Texture> mIDTable;
		NameTable mNameTable;
	};
	 
}