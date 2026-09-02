#include "EditorDirector/ImGuiSystem.h"

#include "EditorDirector/EditorProjectManager.h"
#include "EditorDirector/Generated/BuildInfo.h"
#include "EditorDirector/ImGui/imgui.h"
#include "EditorDirector/ImGui/imgui_impl_dx12.h"
#include "EditorDirector/ImGui/imgui_impl_win32.h"
#include "EditorDirector/ImGui/imstb_truetype.h"
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/Font.h>
#include <CoreAsset/IntermediateAsset.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/MaterialManager.h>
#include <CoreAsset/Mesh.h>
#include <CoreAsset/StaticMesh.h>
#include <CoreAsset/Texture.h>
#include <CoreAsset/TextureManager.h>
#include <CoreDevice/D3DCoreDevice.h>
#include <D3DGpuResourceManager/D3DGpuDescriptorHeapManager.h>
#include <D3DGpuResourceManager/D3DGpuResource.h>
#include <D3DGpuResourceManager/D3DGpuResourceManager.h>
#include <D3DGpuResourceManager/GRMPtr.h>
#include <D3DGpuResourceManager/IGpuResource.h>
#include <EditorDirector/EditorAssetManager.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <LogicalFileSystem/LogicalFolder.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderSystem/D3DMaterialManager.h>
#include <RenderSystem/D3DRenderSystem.h>
#include <RenderSystem/D3DWindowRenderData.h>
#include <RenderSystem/D3DWindowRenderManager.h>
#include <RenderSystem/IRenderSystem.h>
#include <memory.h>

struct ExampleDescriptorHeapAllocator
{
    /* ID3D12DescriptorHeap* Heap = nullptr;
     D3D12_DESCRIPTOR_HEAP_TYPE  HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
     D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu;
     D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu;
     UINT                        HeapHandleIncrement;
     ImVector<int>               FreeIndices;*/

    D3DGRM::D3DGpuResourceManager *mGpuResourceManager;
    void Create(D3DGRM::D3DGpuResourceManager *gpuResourceManager)
    {
        // IM_ASSERT(Heap == nullptr && FreeIndices.empty());
        // Heap = heap;
        // D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
        // HeapType = desc.Type;
        // HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
        // HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
        // HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
        // FreeIndices.reserve((int)desc.NumDescriptors);
        // for (int n = desc.NumDescriptors; n > 0; n--)
        //     FreeIndices.push_back(n - 1);

        mGpuResourceManager = gpuResourceManager;
    }
    void Destroy()
    {
        /* Heap = nullptr;
         FreeIndices.clear();*/
    }
    void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE *out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE *out_gpu_desc_handle)
    {
        /*IM_ASSERT(FreeIndices.Size > 0);
        int idx = FreeIndices.back();
        FreeIndices.pop_back();*/

        D3DGRM::D3DDescriptorHandle handle = mGpuResourceManager->GetCSUHeapManager()->Allocate();

        out_cpu_desc_handle->ptr = handle.mCpuDescriptorHandle.ptr;
        out_gpu_desc_handle->ptr = handle.mGpuDescriptorHandle.ptr;
    }
    void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
    {
        D3DGRM::D3DDescriptorHandle handle;

        handle.mCpuDescriptorHandle = out_cpu_desc_handle;
        handle.mGpuDescriptorHandle = out_gpu_desc_handle;

        mGpuResourceManager->GetCSUHeapManager()->Free(handle);
    }
};

Quad::ImGuiSystem *Quad::ImGuiSystem::GetInstance()
{
    static ImGuiSystem instance;
    return &instance;
}

Quad::ImGuiSystem::ImGuiSystem() {}

Quad::ImGuiSystem::~ImGuiSystem() {}
void Quad::ImGuiSystem::Initialize() {}

void Quad::ImGuiSystem::InitPlatform(HWND hwnd, ID3D12Device *device, ID3D12CommandQueue *commandQueue) {}

void Quad::ImGuiSystem::SetDependency(QuadLF::LogicalFileSystem *logicalFileSystem,
                                      CoreAsset::TextureManager *textureManager,
                                      CoreAsset::MaterialManager *materialManager,
                                      EditorAssetManager *editorAssetManager)
{
}

void Quad::ImGuiSystem::Draw()
{

    // D3DRender::D3DRenderSystem *renderSystem =
    //     static_cast<D3DRender::D3DRenderSystem *>(Render::IRenderSystem::GetInstance());

    // ID3D12GraphicsCommandList *commandList = mCommandList;
    // ID3D12DescriptorHeap *heaps[] = {m_ImGuiSrvHeap};

    // ID3D12CommandAllocator *commandAlloc = mCommandAllcator;
    //// commandList->Reset(commandAlloc, nullptr);

    // D3DRender::D3DWindowRenderManager *d3dWindowRenderManager = D3DRender::D3DWindowRenderManager::GetInstance();
    // std::shared_ptr<D3DRender::D3DWindowRenderData> windowRenderData =
    //     d3dWindowRenderManager->GetWindowRenderData(mHwnd);

    // GRM::GRMPtr backBuffer = windowRenderData->GetBackBuffer(windowRenderData->GetCurrentBackBufferIndex());

    // D3DGRM::D3DGpuResource *d3dBackBuffer = static_cast<D3DGRM::D3DGpuResource *>(backBuffer.getResource());

    // D3DGRM::D3DDescriptorHandle backBufferDescriptorHandle;
    // d3dBackBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, backBufferDescriptorHandle);
    // commandList->OMSetRenderTargets(1, &backBufferDescriptorHandle.mCpuDescriptorHandle, false, nullptr);

    // D3D12_VIEWPORT viewport = {
    //     0,
    // };
    // RECT clientRect;
    // GetClientRect(mHwnd, &clientRect);

    // viewport.Width = clientRect.right;
    // viewport.Height = clientRect.bottom;

    // commandList->RSSetViewports(1, &viewport);

    // ImGui_ImplDX12_NewFrame();
    // ImGui_ImplWin32_NewFrame();

    // ImGui::NewFrame();

    // ImGui::Begin("Texture Test");

    // ImGui::Text("안녕하세요");

    // ImGui::End();

    // DrawEditorBuildInfo();
    // DrawLogicalFileSystem();
    // DrawSelectAsset();
    // DrawCreationMaterialPanel();
    // DrawSavePanel();

    // ImGui::Render();

    // commandList->SetDescriptorHeaps(1, heaps);
    // ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

    //// commandList->Close();
    ////// ID3D12CommandList *commandLists[] = {commandList};
    //// mCommandQueue->ExecuteCommandLists(1, commandLists);
}

void Quad::ImGuiSystem::DrawEditorBuildInfo() {}

void Quad::ImGuiSystem::DrawLogicalFileSystem() {}

void Quad::ImGuiSystem::DrawLogicalFolder(QuadLF::LogicalFolder *logicalFolder) {}

void Quad::ImGuiSystem::DrawSelectAsset() {}

void Quad::ImGuiSystem::DrawCreationMaterialPanel() {}

void Quad::ImGuiSystem::DrawSavePanel() {}
