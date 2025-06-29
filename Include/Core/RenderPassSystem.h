#pragma once



#include"Predeclare.h"
#include<vector>
#include<array>
#include<D3d12.h>

#include"Object/ObjectType.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	struct RenderPassItem
	{
		RenderPassTwo* mRenderPass;
		std::vector<std::vector<RenderItem*>> mRenderItemVector;	//mapLayer별로 구분
	};

	class RenderPassCommand;

	class MapLayerRenderData;

	class CORE_API_LIB RenderPassSystem
	{
		

	public:

		RenderPassSystem() = default;
		~RenderPassSystem() = default;


		void Initialize(GraphicCommand * graphicsCommandObject,DescriptorHeapManagerMaster * descriptorHeapManagerMaster);

		const RenderPassItem& GetRenderPassItem(RenderPassTwo* renderPass) const;
		void AddRenderItem(RenderPassTwo* renderPass, RenderItem* renderItem, int mapLayerIndex);
		const std::vector<RenderPassItem>& GetRenderPassItemVector(ERenderPassID id) const;

		void UploadData(const PassData& passData);
		//void SetRenderSettingItem(RenderSettingItem* renderSettingItem);
		void SetMapLayerVector(const std::vector<MapLayer> & mapLayerVector, const  std::vector<MapLayerRenderData>& mapLayerRenderDataVector);

		const std::vector<RenderSettingItem>& GetMapLayerSettingVector() const;




		void SetDefaultRenderTargetAndDepthStencil(RenderTargetTexture* renderTargetTexture, Texture* depthStencilBuffer);
		



		void Draw(bool stencilDrawFlag);

		void Reset();


		void SetCurrentRenderTargetDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv );

		//RenderSettingItem& GetRenderSettingItem();
	private:

		void BeforeDraw(int mapLayerIndex,bool stencilDrawFlag);

		void PreRenderPass(const RenderPassItem& renderPass, int mapLayerindex);
		void PostRenderPass(const RenderPassItem& renderPass, int mapLayerIndex, bool stencilDrawFlag);
		void ExecuteRenderPassCommand(Effect* effect ,const std::vector<RenderPassCommand*> & renderPassCommandVector, int mapLayerIndex);

		void AfterDraw(int mapLayerIndex,bool stencilDrawFlag);

		void UploadDataToRenderPassShaderResource(RenderPassTwo* renderPass, std::vector<RenderItem*>& renderItemVector,
			const PassData& passData);


		void DrawRenderPass(const RenderPassItem& renderPassItem, int mapLayerIndex);
		void BindShaderResource(const std::vector<ShaderResource*>& shaderResourceVector, RenderItem* renderItem,
			int elementIndex, bool objectOrPassFlag);


		void DrawRenderItem(RenderItem* renderItem);
		void DrawLineBaseRenderItem(RenderItem* renderItem);


		void UploadDataToConstantBuffer(ShaderResourceConstantBuffer * shaderConstantBuffer, RenderItem* renderItem,const PassData& passData);
		void UploadLightStructData(ShaderResourceConstantBuffer* shaderResourceConstantBuffer, int variableIndex, const ShaderResourceStructTypeDesc* pShaderResourceStructTypeDesc, int elementNum, int structOffset, const PassData& passData, ELightType lightType);



		void SetScissorRect(const RenderItem& renderItem, int mapLayerIndex);


	private:

		GraphicCommand* mGraphicsCommandObject;
		DescriptorHeapManagerMaster* mDescriptorHeapManagerMaster;
			

		std::array<std::vector<RenderPassItem>, 10> mRenderPassItemContainer;


		//RenderSettingItem* mRenderSettingItem;




		std::vector<RenderSettingItem> mMapLayerSettingItemVector;



		RenderTargetTexture* mDefaultRenderTargetTexture =nullptr;
		Texture* mDefaultDepthStencilBuffer =nullptr; 

		D3D12_CPU_DESCRIPTOR_HANDLE mCurrentDefaultRtv;
		D3D12_CPU_DESCRIPTOR_HANDLE mCurrentDefaultDsv;
		D3D12_VIEWPORT mViewport;
	};


}