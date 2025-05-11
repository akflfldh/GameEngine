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


#include"Core/CoreDllExport.h"
//ui 엔티티들이 들어오는데 
//일반적인 엔티티들과는 다른 그래픽파이프라인, 카메라, viewproj행렬 등을 사용할것이다
//물론 일반적인 엔티티들,ui엔티티들 모두 각각의 다른 그래픽파이프라인을 사용할수있기에 (보통 메터리얼이 관련될거고,)
//ui ,일반적 엔티티들은 서로다른 viewproj, 서로다른/

namespace Quad
{
	
	struct CORE_API_LIB RenderItem
	{
		SceneElement* mSceneElement;
		EObjectType mObjectType;
		//SubMesh mSubMesh;
		//const ModelSubMesh * mSubMesh;
		const BaseModelSubMesh* mSubMesh;
		
		int mInstanceCount = 1;
		int mStencilRefValue = 0;	//인스턴싱의경우에도 모든인스턴스들에게 동일한값적용
		//instancing
		int mSubMeshIndex = -1;
		// shader resource structuredbuffer ,bufferindex
		//std::vector<std::pair<ShaderResourceStructuredBuffer*, int>> mStructuredBufferIndexVector;
		//bool instancing flag;

		//인덱스를 사용하는지 안하는지에대한여부도있어야된다.
		bool mIndexBufferUsageFlag = true;


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
		D3D12_VIEWPORT mViewPort;
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
	//	void DrawEffect(const Effect* effect, const std::vector<RenderItem*>& renderItemVector,const RenderSettingItem & renderSettingItem, ESystemType systemType);
		//void DrawRenderPass(const Effect* effect,const RenderPassTwo* renderPass, const std::vector<RenderItem*> & renderItemVector,
		//	const RenderSettingItem& renderSettingItem, ESystemType systemType);
		/*void BindShaderResource(const std::vector<ShaderResource*>& shaderResourceVector, RenderItem* renderItem,
			int elementIndex, ESystemType systemType, bool objectOrPassFlag);*/

		void AddEntity(Object* entity);
		void SetEntityVector(const std::vector<Object*>& entityVector, ESystemType systemType, int mapLayerIndex);
		void SetMapLayerVector(const std::vector<MapLayer>& mapLayerVector, ESystemType systemType);



	//	void SetUiEntityVector(const std::vector<Object*>& entityVector);
		void UploadEntityData();
		void UploadEntityDataPerSystem(ESystemType systemType);

		//void UploadEntityDataConstantBuffer(ShaderResourceConstantBuffer* shaderResourceConstantBuffer,const std::vector<RenderItem*> &renderItemVectorPerEffect,
		//	const PassData& passData, ESystemType systemType);
		//void IncreaseConstantBufferViewIndexOffset(ShaderResourceConstantBuffer* shaderResourceConstantBuffer, ESystemType systemType);

		void ResetResource();//PreUpdate에서 호출,매프레임렌더링하기전 데이터들을초기화수행ex)renderItem을 다반환,viewIndex=0초기화
		


		

		void SetRenderMapItem(RenderMapItem& renderMapItem);
		void SetRenderSettingItem(RenderSettingItem& renderSettingItem, ESystemType systemType);
		void SetRenderUiSettingItem(RenderSettingItem& renderUiSettingItem);
		void OnResize(UINT clientWidth, UINT clientHeight);

		void SetBackgroundColor(float r, float g, float b, float a);
		

		void SetRenderState(bool state);
		bool GetRenderState()const;

		void AddEffect(const Effect& effect, ESystemType systemType);
		Effect* GetEffect(const std::string& name, ESystemType systemType)const;

		void SetColliderWorldRenderState(bool state);
		bool GetColliderWorldRenderState() const;


		void SetColliderDrawFlag(bool flag); 
		bool GetColliderDrawFlag() const;

	private:
		bool InitD3d();

		void CreateSwapChain();
		void CreateSwapChainDepthStencilBufferAndView();
		//void CreateDescriptorHeaps();
		void CreateSwapChainRtv();
		//void CreateSwapchainDsv();		//깊이*스텐실버퍼도 함께만든다.


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


	private:
		RenderItem* FindInstancingRenderItem(const std::vector<RenderItem*> & renderItemVector, const Mesh* mesh, int subMeshIndex);
		


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
		Microsoft::WRL::ComPtr<ID3D12Resource> mSwapchainDepthStencilBuffer;

		ViewIndex mSwapChainBufferRtvIndex[mSwapchainBufferCount];
		ViewIndex mSwapChainDsvIndex;
		DXGI_FORMAT mBackBufferForamt = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mSwapchainDepthStencilBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		UINT m4xmsaaQuality;

		D3D12_VIEWPORT mClientViewPort;
		RECT mScissorRect;

		UINT mClientWidth;
		UINT mClientHeight;

		FLOAT mBackgroundColor[4];

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
		std::unordered_map<ESystemType,	std::unordered_map<std::string, Effect*>> mEffectTablePerSystem;

		std::vector<RenderItem*> mRenderItemVector;//전체렌더아이템을유지하고 해제하기 위한컨테이너역할


		std::unordered_map<ESystemType ,std::vector<LightItem*>>  mLightItemVectorPerSystem;
		DestructingPoolAllocator<RenderItem> * mRenderItemPoolAllocator;
		DestructingPoolAllocator<LightItem> * mLightItemPoolAllocator;

		int mWindowType;
		bool mRenderState=true;		//true :render , false : stop


		bool mColliderWorldRenderState =false;

		bool mColliderDrawFlag = false;

	};

}