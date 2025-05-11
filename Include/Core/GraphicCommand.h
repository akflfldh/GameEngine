#pragma once


#include"header.h"
#include"Utility/Utility.h"
#include"Core/CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB GraphicCommand
	{
	public:
		GraphicCommand() = default;
		~GraphicCommand() = default;

		void Initialize(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator>commandAllocator,
			Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
			Microsoft::WRL::ComPtr<ID3D12Fence> fence);

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetGraphicsCommandList() const;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() const;


		HRESULT ResetCommandList(ID3D12PipelineState* pipelineState);
		void ResetCommandAllocator();

		void ExecuteCommandList();

		void FlushCommandQueue();

		bool GetCloseState() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		UINT mCurrentFence;
		
		bool mCloseState;

	};

}
