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
		std::vector<std::vector<RenderItem*>> mRenderItemVector;
	};


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
		void SetMapLayerVector(const std::vector<MapLayer> & mapLayerVector);


		void Draw(bool stencilDrawFlag);

		void Reset();


		void SetCurrentRenderTargetDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv );

		//RenderSettingItem& GetRenderSettingItem();
	private:

		void BeforeDraw(int mapLayerIndex,bool stencilDrawFlag);
		void AfterDraw(int mapLayerIndex,bool stencilDrawFlag);

		void UploadDataToRenderPassShaderResource(RenderPassTwo* renderPass, std::vector<RenderItem*>& renderItemVector,
			const PassData& passData);


		void DrawRenderPass(const RenderPassItem& renderPassItem, int mapLayerIndex);
		void BindShaderResource(const std::vector<ShaderResource*>& shaderResourceVector, RenderItem* renderItem,
			int elementIndex, bool objectOrPassFlag);


		void DrawEntityRenderItem(RenderItem* renderItem);
		void DrawLineBaseRenderItem(RenderItem* renderItem);


		void UploadDataToConstantBuffer(ShaderResourceConstantBuffer * shaderConstantBuffer, RenderItem* renderItem,const PassData& passData);
		void UploadLightStructData(ShaderResourceConstantBuffer* shaderResourceConstantBuffer, int variableIndex, const ShaderResourceStructTypeDesc* pShaderResourceStructTypeDesc, int elementNum, int structOffset, const PassData& passData, ELightType lightType);

	private:

		GraphicCommand* mGraphicsCommandObject;
		DescriptorHeapManagerMaster* mDescriptorHeapManagerMaster;
				//占싹댐옙 10 占쌤곤옙占?크占쏙옙 占쏙옙占쏙옙占싻쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙.
				//占쏙옙 占쌤계에占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙占싻쏙옙占쏙옙占쏙옙 占쏙옙載ο옙占쏙옙甄占?占쎌선占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙 
		std::array<std::vector<RenderPassItem>, 10> mRenderPassItemContainer;


		//RenderSettingItem* mRenderSettingItem;




		std::vector<RenderSettingItem> mMapLayerSettingItemVector;

		D3D12_CPU_DESCRIPTOR_HANDLE mCurrentDefaultRtv;
		D3D12_CPU_DESCRIPTOR_HANDLE mCurrentDefaultDsv;
		D3D12_VIEWPORT mViewport;
	};


}