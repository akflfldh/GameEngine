#pragma once


//mapLayer 렌더링시 필요한 data들을 가지고있는 클래스

#include"header.h"
#include"Core/CoreDllExport.h"



namespace Quad
{
	class Effect;
	class Texture;
	class RenderTargetTexture;


	class CORE_API_LIB MapLayerRenderData
	{
	public:
		
												//<effect name , effect>
		using EffectTable = std::unordered_map<std::string, Effect*>;


		MapLayerRenderData();
		~MapLayerRenderData();



		void SetDefaultRenderTarget(RenderTargetTexture *  defaultRenderTarget);
		void SetDefaultDepthStencilBuffer(Texture * defaultDepthStencilBuffer);

		void AddEffect(Effect* effect);


		void SetViewport(D3D12_VIEWPORT viewport);
		void SetViewportGlobal(D3D12_VIEWPORT viewport);


		RenderTargetTexture * GetDefaultRenderTarget() const;
		Texture* GetDefaultDepthStencilBuffer() const;

		D3D12_VIEWPORT GetViewport() const;
		D3D12_VIEWPORT GetViewportGlobal() const;


		Effect* GetEffect(const std::string& name) const;

		void OnResize(UINT clienWidth, UINT clientHeight);

		//매 렌더링시작 혹은 종료시마다 reset을 수행하여 다음번 draw가 올바르게 수행되도록
		void ResetEffectResource();

	private:
		using EffectTable = std::unordered_map<std::string, Effect*>;
		RenderTargetTexture *  mDefaultRenderTarget;
		Texture* mDefaultDepthStencilBuffer;
		EffectTable mEffectTable;

		D3D12_VIEWPORT mViewport;
		D3D12_VIEWPORT mViewportGlobal;



	};

}
