#include "Core/GraphicCommand.h"
namespace Quad
{
	void GraphicCommand::Initialize(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
		Microsoft::WRL::ComPtr<ID3D12Fence> fence)
	{
		mCommandList = commandList;
		mCommandAllocator = commandAllocator;
		mCommandQueue = commandQueue;
		mFence = fence;
		mCurrentFence = 0;

		mCloseState = true;
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GraphicCommand::GetGraphicsCommandList() const
	{
		return mCommandList;
	}

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> GraphicCommand::GetCommandQueue() const
	{
		return mCommandQueue;
	}

	HRESULT GraphicCommand::ResetCommandList(ID3D12PipelineState* pipelineState)
	{
		mCloseState = false;
		return mCommandList->Reset(mCommandAllocator.Get(), pipelineState);
		
	}

	void GraphicCommand::ResetCommandAllocator()
	{
		mCommandAllocator->Reset();
	
	}

	void GraphicCommand::ExecuteCommandList()
	{
		mCommandList->Close();
		ID3D12CommandList* commandLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
		mCloseState = true;

	}

	void GraphicCommand::FlushCommandQueue()
	{

		mCurrentFence++;
		ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

		if (mFence->GetCompletedValue() < mCurrentFence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));//이울타리지점에 도달하면 event발생

			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
			
		}
		//mCloseState = true;
	}

	bool GraphicCommand::GetCloseState() const
	{
		return mCloseState;
	}


}