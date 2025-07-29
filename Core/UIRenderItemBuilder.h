#pragma once




#include<IRenderSystem.h>
#include<UISystem.h>
#include<RenderType.h>
#include<Asset/Mesh/MeshType.h>


namespace GRM { class IGpuResource; class IGpuResourceManager; }
namespace UI { class UIElement; }

namespace Quad
{
	//RenderItem분류 기준이된다.


	//빌더에서 내부적으로 Element들을 정렬하기위해 사용할 Batch데이터모음
	struct UIElementBatchKey
	{
		Render::RenderChannelID mRenderChannelID;
		Render::MaterialID mMaterialID;
		Render::ScissorRect mScissorRect;

		bool operator<(const UIElementBatchKey& batchKey) const
		{
			if (mRenderChannelID < batchKey.mRenderChannelID) return mRenderChannelID < batchKey.mRenderChannelID;

			if (mMaterialID != batchKey.mMaterialID) return mMaterialID < batchKey.mMaterialID;

			if (mScissorRect.Left != batchKey.mScissorRect.Left) return mScissorRect.Left < batchKey.mScissorRect.Left;
			if (mScissorRect.Right != batchKey.mScissorRect.Right) return mScissorRect.Right < batchKey.mScissorRect.Right;
			if (mScissorRect.Top != batchKey.mScissorRect.Top) return mScissorRect.Top < batchKey.mScissorRect.Top;
			if (mScissorRect.Bottom != batchKey.mScissorRect.Bottom) return mScissorRect.Bottom < batchKey.mScissorRect.Bottom;


			return false;

		}

		bool operator ==(const UIElementBatchKey& batchKey) const
		{
			if (mRenderChannelID != batchKey.mRenderChannelID) return false;

			if (mMaterialID != batchKey.mMaterialID) return false;

			if (mScissorRect.Left != batchKey.mScissorRect.Left) return false;
			if (mScissorRect.Right != batchKey.mScissorRect.Right) return false;
			if (mScissorRect.Top != batchKey.mScissorRect.Top) return false;
			if (mScissorRect.Bottom != batchKey.mScissorRect.Bottom) return false;

			return true;
		}


	};

	//빌더에서 내부적으로 렌더아이템을 구축하기위해서 사용할 UIElement 데이터모음
	struct UIElementContext
	{
		UI::UIElement* mUIElement;
		UIElementBatchKey mBatchKey;

	};



	


	


	class UIRenderItemBuilder
	{

		//지금 머터리얼 별로만 분류하였는데
		//ScissorRect라는 기준도 추가해야한다.
		using MaterialRenderItemTable = std::unordered_map<Render::MaterialID, Render::RenderItem> ;
	public:
		static UIRenderItemBuilder* GetInstance();
		
		UIRenderItemBuilder(Render::IRenderSystem* renderSystem, UI::UIManager * uiManager, GRM::IGpuResourceManager
		* gpuResourceManager);
		~UIRenderItemBuilder();





		//렌더아이템을 생성한다.
		 //추가적인 정보를 더 넘겨줘야해 아닐수도있고
		//비동기가 가능할수도
		void SubmitUIChannel(const UI::UIChannelContext& uiChannelContext , Render::RenderChannelID renderChannelID /* 연결되어있는 렌더시스템의 채널정보도 같이 */);


		//만들어진 각채널의모든 렌더아이템들을 렌더시스템에게 전달한다.
		void SubmitRenderItemAll();

	private:


		// UIElement의 버텍스를 렌더아이템에 반영하는 내부 함수
		//DestVertexVector 뒤로 버텍스들이 추가된다.(4개)
		void AddVertexToBatchedVector(Render::RenderItem& oRenderItem , UI::UIElement * uiElement);
		//indexoffset : 새로추가하는 ui의 인덱스값이 그 렌더아이템의 시작 index로부터 얼만큼떨어져있는지 
		void AddIndexToBatchedVector(Render::RenderItem& oRenderItem, uint32_t indexoffset);

		void InitRenderItem(Render::RenderItem& oRenderItem, size_t vertexOffset, size_t indexOffset,
			const UIElementContext& uiElementContext);


	private:
		static UIRenderItemBuilder* mInstance;

	private:
		Render::IRenderSystem* mRenderSystem;
		UI::UIManager* mUiManager;
		GRM::IGpuResourceManager* mGpuResourceManager;
		GRM::IGpuResource* mVertexBufferGpu;
		GRM::IGpuResource* mIndexBufferGpu;
		
		


		

		//매프레임마다 초기화된다.
								//렌더채널별  <  머터리얼 - 렌더아이템> 테이블
		std::unordered_map<Render::RenderChannelID, std::vector<UIElementContext>>  mUIElementContextVectorPerRenderChannel;
		std::vector<UIVertex> mBatchedVertexVector;
		std::vector<uint32_t> mBatchedIndexVector;

	};

}
