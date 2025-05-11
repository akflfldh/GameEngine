#pragma once



#include"Predeclare.h"
#include"EditorDirector/DockingWindowController.h"

#include"Singleton.h"
namespace Quad
{
	class FileUiWindowContoller:public DockingWindowController,public Singleton<FileUiWindowContoller>
	{
	public:
		FileUiWindowContoller(RenderSystem * renderSystem) ;
		~FileUiWindowContoller() = default;


		virtual void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList,
			MeshManager * meshManager, MaterialManager * materialManager, TextureManager * textureManager,
			MapManager * mapManager, TaskWindow * window,
			System * gameWorldSystem, WindowChromeSystem * windowLayoutSystem,
			WindowDockingSystem * windowDockingSystem,
			DescriptorHeapManagerMaster * descriptorHeapManagerMaster);

		virtual void DragAndDropFile(const std::vector<std::string>& dropFileVector) override;
		//virtual void DragAndDropFile(std::vector<std::string>&& dropFileVector) override;

		void HandleEvent(Event* pEvent) override;
	private:
		void HandleLoadFileResponseEvent(Event* pEvent);


	};


}

