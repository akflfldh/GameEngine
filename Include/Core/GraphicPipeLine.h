#pragma once

#include"header.h"
#include"DescriptorHeapType.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB GraphicPipeLine
	{
	public:
		GraphicPipeLine() = default;
		~GraphicPipeLine() = default;

		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList,
			const std::string& effectFile,
			const std::string& vs, const std::string& ps);

		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState()const;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature()const;

	private:



	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mGraphicsCommandList;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
		//RootSignature* mRootSignature;


		//ViewIndex mSamplerViewIndex;

		std::string mEffectFile;



	};

}