#pragma once


#include"Predeclare.h"
#include"header.h"
#include"HeapManager/DescriptorHeapManagerMaster.h"

#include"Map/Map.h"
#include"GraphicPipeLine.h"
#include"Buffer/UploadBuffer.h"
#include"Shader/ShaderResourceType.h"
#include"Allocator/DestructingPoolAllocator.h"
#include"RenderPassSystem.h"
#include"Asset/Texture/RenderTargetTexture.h"



#include"Asset/Mesh/MeshType.h"
#include"Core/MapLayerRenderData.h"

#include"Core/CoreDllExport.h"
//ui 엔티티들이 들어오는데 
//일반적인 엔티티들과는 다른 그래픽파이프라인, 카메라, viewproj행렬 등을 사용할것이다
//물론 일반적인 엔티티들,ui엔티티들 모두 각각의 다른 그래픽파이프라인을 사용할수있기에 (보통 메터리얼이 관련될거고,)
//ui ,일반적 엔티티들은 서로다른 viewproj, 서로다른/



namespace Quad
{
	class IMeshComponent;

	struct CORE_API_LIB RenderItem
	{
		IMeshComponent* mMeshComponent;
		int mInstanceCount = 1;
		int mStencilRefValue = 0;	//인스턴싱의경우에도 모든인스턴스들에게 동일한값적용
	
		int mSubMeshIndex = -1;

		//인덱스를 사용하는지 안하는지에대한여부도있어야된다.
		bool mIndexBufferUsageFlag = true;

	
		bool mScissorRectUsageFlag = false;
		RECT mScissorRect;

	};

	//Light를 위한 구조체가필요할거같다.
	//upload할떄 light정보들도 같이 넘겨서 업로드에서 사용할수있도록
	//렌더아이템은 렌더링되는놈들이이니깐submesh
	//조명이 렌더링되는건아니니깐

	struct LightItem
	{
		Light* mLight;
	};




	class Camera;
	class Entity;

	struct RenderSettingItem
	{

		Camera* mCamera = nullptr;
		D3D12_VIEWPORT mViewPort;			//최종 후면버퍼에대한 mapLayer의 렌더타켓 전역 viewport	
		D3D12_VIEWPORT mViewPortLocal;		//topleftX,Y가 0인 mapLayer의 렌더타켓의 local viewport
		RECT mScissorRectLocal;				//viewportLocal에 맞는 값을 가진다.

		RenderTargetTexture* mRenderTarget = nullptr;
		Texture* mDepthStencilBuffer = nullptr;
	};



	class CORE_API_LIB RenderSystem
	{
	private:
	



	public:
		RenderSystem();
		~RenderSystem() ;

		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
			Microsoft::WRL::ComPtr<IDXGIFactory4> factory,
			GraphicCommand * graphicCommandObject,
			HWND hWnd, UINT clientWidth, UINT clientHeight,
			DescriptorHeapManagerMaster* descriptorHeapManagerMaster, int windowID);


		void PreUpdate();
		void Update();
		void Draw();

		void SetRenderItemToPassSystem(const std::vector<RenderItem*>& renderItemVector, ESystemType systemType, int mapLayerIndex);

		//renderPassSystem에 mapLayer에대한 값을 설정한다.
		void SetMapLayerVector(const std::vector<MapLayer>& mapLayerVector, ESystemType systemType);


		//mapLayerRenderData에 viewport값을 설정한다.
		void SetMapLayerViewportRenderData(const std::vector<MapLayer>& mapLayerVector, ESystemType systemType);


	//	void SetUiEntityVector(const std::vector<Object*>& entityVector);
		void UploadEntityData();
		void UploadEntityDataPerSystem(ESystemType systemType);


		void ResetResource();//PreUpdate에서 호출,매프레임렌더링하기전 데이터들을초기화수행ex)renderItem을 다반환,viewIndex=0초기화
		


		

		void SetRenderMapItem(RenderMapItem& renderMapItem);
		void SetRenderSettingItem(RenderSettingItem& renderSettingItem, ESystemType systemType);
		void SetRenderUiSettingItem(RenderSettingItem& renderUiSettingItem);
		void OnResize(UINT clientWidth, UINT clientHeight);
		

		void SetBackgroundColor(float r, float g, float b, float a);
		

		void SetRenderState(bool state);
		bool GetRenderState()const;

		void AddEffect(const Effect& effect, ESystemType systemType);
		Effect* GetEffect(const std::string& name, ESystemType systemType, int mapLayerIndex)const;

		void SetColliderWorldRenderState(bool state);
		bool GetColliderWorldRenderState() const;


		void SetColliderDrawFlag(bool flag); 
		bool GetColliderDrawFlag() const;



		void NotifyCreatingMapLayer(ESystemType systemType, D3D12_VIEWPORT viewportLocal, D3D12_VIEWPORT viewportGlobal);
		void NotifyResizeMapLayer(ESystemType systemType, int mapLayerIndex ,D3D12_VIEWPORT viewportLocal, D3D12_VIEWPORT viewportGlobal);

		void ReigsterDefaultEffect(ESystemType systemType, const std::string& effectName);

	private:
		bool InitD3d();

		void CreateSwapChain();
		void CreateSwapChainDepthStencilBufferAndView();
		//void CreateDescriptorHeaps();
		void CreateSwapChainRtv();
		//void CreateSwapchainDsv();		//깊이*스텐실버퍼도 함께만든다.

		RenderTargetTexture* CreateDefaultRenderTargetTexture(int width, int height);
		Texture* CreateDefaultDepthStencilBuffer(int width, int height);


		//mapLayer들의 rendertarget의 resource를 만드는 메서드
		void CreateMapLayerDefaultRenderTargetTextureResource(RenderTargetTexture* renderTargetTexture,
			int width, int height);
		void CreateMapLayerDefaultRenderTargetTextureResourceAll();


		void CreateMapLayerDefaultDepthStencilBufferResource(Texture* depthStencilBuffer,int width, int height);
		//mapLayer들의 깊이버퍼 resource를 만드는 메서드
		void CreateMapLayerDefaultDepthStencilBufferResourceAll();



		//renderTargetTexture 자체는 살아있다 .
		void ReleaseMapLayerDefaultRenderTargetTextureResource(RenderTargetTexture *);
		void ReleaseMapLayerDefaultDepthStencilBufferResource(Texture* depthstencilBuffer);


		//renderTargetTexture class자체는 살아있다.
		void ReleaseMapLayerDefaultRenderTargetTextureResourceAll();
		void ReleaseMapLayerDefaultDepthStencilBufferResourceAll();



		//mapLayer들의 default renderTarget, depthstencilBuffer에 대해서 resize
	



		ID3D12Resource* GetCurrentBackBuffer()const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView()const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilBufferView() const;
		virtual void MouseDown(WPARAM wParam, int x, int y) {};
		virtual void MouseUp(WPARAM wParam, int x, int y) {};
		virtual void MouseMove(WPARAM wParam, int x, int y) {};
		float GetAspectRatio() const;



		void BeforeDraw(bool preStencilDrawFlag);
		void AfterDraw();



		void ResetEffectResource(Effect * effect);
		void ResetRenderPassResource(const std::vector<RenderPassTwo*>& renderPassVector);



		void OnResizeMapLayerRenderData(UINT clientWidth, UINT clientHeight);

		void SetDefaultRenderTargetAndDepthStencilToRenderPassSystem();


		void InitializeDefaultRenderItem();



		void SetRenderItemToPassSystem_FixedPass(RenderPassSystem* renderPassSystem,
			const std::vector<RenderItem*>& renderItemVector, int mapLayerIndex, ESystemType systemType);
		void SetRenderItemToPassSystem_CommonPass(RenderPassSystem* renderPassSystem,
			const std::vector<RenderItem*>& renderItemVector, int mapLayerIndex, ESystemType systemType);




		//renderItem 생성시, scissorRect컴포넌트가있다면 scissorRectUsageFlag를 true로  ,rect를설정한다.
		void SetScissorRectUsage(RenderItem& renderItem, Object* object);

		

	private:
		RenderItem* FindInstancingRenderItem(const std::vector<RenderItem*> & renderItemVector, const Mesh* mesh, int subMeshIndex);
		
		//effect별 텍스처들을 생성
		void CreateEffectTexture(Effect* effect);

		void InitMapLayerRenderDataTable();

		void CreateRenderItem(const std::vector<Object*>& objectVector, std::vector<RenderItem*>& oRenderItemVector);
			
	private:
		Microsoft::WRL::ComPtr<IDXGIFactory4> mFactory;
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		GraphicCommand* mGraphicCommandObject;
		UINT64 mCurrentFence = 0;

		//SWAP CHAIN
		//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSwapchainRtvHeap;
		//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>mSwapchainDsvHeap;
		HWND mHwnd;

		Microsoft::WRL::ComPtr<IDXGISwapChain>mSwapchain;
		static const int mSwapchainBufferCount = 2;
		int mCurrBackBufferIndex = 0;

		Microsoft::WRL::ComPtr<ID3D12Resource>mSwapchainBuffer[mSwapchainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> mSwapchainDepthStencilBufferResource;
		//Microsoft::WRL::ComPtr<ID3D12Resource> mDefaultRenderTargetTextureResource;

		//RenderTargetTexture mDefaultRenderTargetTexture;
		 
		ViewIndex mSwapChainBufferRtvIndex[mSwapchainBufferCount];
		ViewIndex mSwapChainDsvIndex;

	//	ViewIndex mDefaultRenderTargetTextureSrvIndex;
		//ViewIndex mDefaultRenderTargetTextureRtvIndex;


		DXGI_FORMAT mBackBufferForamt = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mSwapchainDepthStencilBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		UINT m4xmsaaQuality;

		D3D12_VIEWPORT mClientViewPort;
		RECT mScissorRect;

		UINT mClientWidth;
		UINT mClientHeight;

		FLOAT mBackgroundColor[4];
		FLOAT mDefaultMapLayerRenderTargetColor[4];

		UINT mRtvdescriptorSize;
		UINT mDsvdescriptorSize;
		UINT mCbvsrvdescriptorSize;
		UINT mSamplerdescriptorSize;
	
	private:
		DescriptorHeapManagerMaster* mDescriptorHeapManagerMaster = nullptr;

#define GAME_PASS_CONSTANT_INDEX 0
#define UI_PASS_CONSTANT_INDEX 1

		//effect별로 
		RenderMapItem mRenderMapItem;
		RenderSettingItem mRenderSettingItem;
		//RenderSettingItem mRenderUiSettingItem;
		RenderSettingItem mRenderWindowLayoutSettingItem;
		RenderSettingItem mRenderWindowDockingSettingItem;

	private:
		//effect별 순서가 필요할수있을것이다.

						//<effect name , render item vector> 

		RenderPassSystem mMainRenderPassSystem;
		//RenderPassSystem mUiRenderPassSystem;
		RenderPassSystem mWindowLayoutRenderPassSystem;
		RenderPassSystem mWindowDockingRenderPassSystem;

		//system별로 effectTable이있어야겠다.
		//0 gameplayWindow, 1, LayouyWindow, 2 WindowDocking
		//( effect table) per mapLayer 
		//std::unordered_map<ESystemType, std::vector<std::unordered_map<std::string, Effect*>>> mEffectTablePerSystem;



		std::unordered_map<ESystemType, std::vector<MapLayerRenderData>> mMapLayerRenderDataTable;


		//system별 effect list 
		std::unordered_map<ESystemType, std::vector<std::string>> mEffectNameVectorPerSystemTable;






		std::vector<RenderItem*> mRenderItemVector;//전체렌더아이템을유지하고 해제하기 위한컨테이너역할


		std::unordered_map<ESystemType ,std::vector<LightItem*>>  mLightItemVectorPerSystem;
		DestructingPoolAllocator<RenderItem> * mRenderItemPoolAllocator;
		DestructingPoolAllocator<LightItem> * mLightItemPoolAllocator;

		int mWindowType;
		bool mRenderState=true;		//true :render , false : stop


		bool mColliderWorldRenderState =false;

		bool mColliderDrawFlag = false;










		static std::unique_ptr<ModelSubMesh> mDefaultEntireRectModelSubMesh;
		static RenderItem mDefaultEntireRectRenderItem;

		
		//모든 렌더시스템 인스턴스들에서사용할 최종 backbuffer로 복사하는 effect 
		static Effect* mRenderTargetToBackBufferEffect; 







	};

}