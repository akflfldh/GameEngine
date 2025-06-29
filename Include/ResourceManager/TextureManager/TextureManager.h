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


		//실제 리소스까지 생성한다 그래서 width,height를 인수로 받는다.
		static Texture* CreateTexture(const std::string & name ,int width, int height);


		static Texture* CreateTextureRandomName(int width, int height);

		//실제 리소스까지 생성한다.
		static RenderTargetTexture* CreateRenderTargetTexture(const std::string& name, int width, int height);

		//실제 리소스까지 생성한다.
		static RenderTargetTexture* CreateRenderTargetTextureRandomName(int width, int height);



		static Texture* CreateDepthStencilBuffer(const std::string& name, int width, int height);

		static Texture* CreateDepthStencilBufferRandomName(int width, int height);


		static Texture* GetTexture(const std::string& name);
		static Texture* GetTexture(unsigned long long id);



		//기존에 이미 생성된 renderTargetTexture에 새로운 내부 리소스를생성해서 준다.
		//비어있는 renderTargetTexture이어야한다. 기존의 내부리소스는 제거된상태이어야함, (렌더시스템에서사용)
		static void CreateRenderTargetTextureResource(RenderTargetTexture* renderTargetTexture, unsigned int width, unsigned int height);
		static void CreateDepthStencilBufferResource(Texture* depthStencilBuffer, unsigned int width, unsigned int height);


		//사이즈를 새롭게 조절해준다.
		//기존의 viewIndex와 내부리소스는 무효화되고
		//새로운값으로 설정
		static void ResizeTexture(Texture* texture, unsigned int width ,unsigned int height);
	

		//내부리소스,view 만제거한다.
		static void ReleaseTextureResource(Texture* texture);




		//static bool  CheckDuplicate(const std::string& name);
		
		//static bool AddTexture(Texture* texture, const std::string& name);


		static void KillTexture(Texture* texture);
		static void KillTexture(unsigned long long id);
		static void KillTexture(const std::string & name);
		static void RemoveDeadTexture();
	//	static void RemoveTextureResource(Texture* texture);

	private:
		TextureFactory mTextureFactory;
		//DeadTable
		std::vector<Texture*> mDeadTable;
		UniqueIDTable<Texture> mIDTable;
		NameTable mNameTable;
	};
	 
}