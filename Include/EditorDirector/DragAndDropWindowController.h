#pragma once





#include"Predeclare.h"
#include"Core/Controller.h"
#include"Object/UiEntity.h"
#include"Core/RenderSystem.h"
#include"Utility/Singleton.h"
namespace Quad
{
	class DragAndDropWindowController:public Controller,public Singleton<DragAndDropWindowController>
	{
	public:
		DragAndDropWindowController() = default;
		~DragAndDropWindowController();

		virtual void Initialize(DragAndDropWindow * dragAndDropWindow, DragAndDropSystem* uiSystem,
			RenderSystem* renderSystem);
		void Update(float deltaTime);
		void EndUpdate(float deltaTime);
		void UploadObjectToRenderSystem();
		void OnResize(unsigned short clientWidth, unsigned short clientHeight);
		void Draw();

		static void RunWindow(ContentItem* contentItem);
		static void EndWindow();
		static bool GetWindowRunningState();
		static bool GetWindowShowState();
		static void SetShowState(bool state);
		static BaseWindow* GetWindow() ;
		static void SetContentItemUiEntity(ContentItemUiEntity * contentItemUiEntity);

	private:
		virtual void Initialize() override;
		DragAndDropWindow* mDragAndDropWindow;
		RenderSystem* mRenderSystem;
		DragAndDropSystem* mUiSystem;
		ContentItemUiEntity* mContentItemUiEntity;//dragAndDropVersion


		DragAndDropUiEntity* mImageEntity;
		Camera* mCamera;
		D3D12_VIEWPORT mViewPort;
		RenderSettingItem mRenderSettingItem;

		bool mWindowRunningState = false;
		bool mWindowShowState = false;

	};



}

