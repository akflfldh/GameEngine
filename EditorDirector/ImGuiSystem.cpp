#include "EditorDirector/ImGuiSystem.h"

#include "EditorDirector/EditorProjectManager.h"
#include "EditorDirector/Generated/BuildInfo.h"
#include "EditorDirector/ImGui/imgui.h"
#include "EditorDirector/ImGui/imgui_impl_dx12.h"
#include "EditorDirector/ImGui/imgui_impl_win32.h"
#include "EditorDirector/ImGui/imstb_truetype.h"
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/MaterialManager.h>
#include <CoreAsset/Texture.h>
#include <CoreAsset/TextureManager.h>
#include <D3DGpuResourceManager/D3DGpuDescriptorHeapManager.h>
#include <D3DGpuResourceManager/D3DGpuResource.h>
#include <D3DGpuResourceManager/D3DGpuResourceManager.h>
#include <D3DGpuResourceManager/GRMPtr.h>
#include <D3DGpuResourceManager/IGpuResource.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <LogicalFileSystem/LogicalFolder.h>
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
void Quad::ImGuiSystem::Initialize()
{
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImFont *font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 16.0f, nullptr,
                                                io.Fonts->GetGlyphRangesKorean());
    io.FontDefault = font;

    /* if (io.Fonts->Fonts.Size == 0)
         io.Fonts->AddFontDefault();*/

    // 키보드 내비게이션 활성화
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling,
                                     // changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;
}

void Quad::ImGuiSystem::InitPlatform(HWND hwnd, ID3D12Device *device, ID3D12CommandQueue *commandQueue)
{
    mHwnd = hwnd;
    mCommandQueue = commandQueue;

    static ExampleDescriptorHeapAllocator g_pd3dSrvDescHeapAlloc;

    // 2. Win32 백엔드 초기화
    ImGui_ImplWin32_Init(hwnd);

    // 3. ImGui용 Descriptor Heap 생성 (SRV용)
    /* D3D12_DESCRIPTOR_HEAP_DESC desc = {};
     desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
     desc.NumDescriptors = 32;
     desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
     desc.NodeMask = 0;

     HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_ImGuiSrvHeap));
     if (FAILED(hr))
     {
         assert(false && "Failed to create ImGui SRV descriptor heap");
         return;
     }

     D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_ImGuiSrvHeap->GetCPUDescriptorHandleForHeapStart();
     D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_ImGuiSrvHeap->GetGPUDescriptorHandleForHeapStart();*/

    D3DGRM::D3DGpuResourceManager *d3dGpuResourceManager =
        static_cast<D3DGRM::D3DGpuResourceManager *>(GRM::IGpuResourceManager::GetInstance());
    g_pd3dSrvDescHeapAlloc.Create(d3dGpuResourceManager);

    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = device;
    init_info.CommandQueue = commandQueue;
    init_info.NumFramesInFlight = 2;
    init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
    // Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
    // (current version of the backend will only allocate one descriptor, future versions will need to allocate more)
    m_ImGuiSrvHeap = d3dGpuResourceManager->GetCSUHeapManager()->GetDescriptorHeap().Get();
    ;
    init_info.SrvDescriptorHeap = m_ImGuiSrvHeap;
    init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo *, D3D12_CPU_DESCRIPTOR_HANDLE *out_cpu_handle,
                                        D3D12_GPU_DESCRIPTOR_HANDLE *out_gpu_handle)
    { return g_pd3dSrvDescHeapAlloc.Alloc(out_cpu_handle, out_gpu_handle); };
    init_info.SrvDescriptorFreeFn =
        [](ImGui_ImplDX12_InitInfo *, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
    { return g_pd3dSrvDescHeapAlloc.Free(cpu_handle, gpu_handle); };
    ImGui_ImplDX12_Init(&init_info);
}

void Quad::ImGuiSystem::SetDependency(QuadLF::LogicalFileSystem *logicalFileSystem,
                                      CoreAsset::TextureManager *textureManager,
                                      CoreAsset::MaterialManager *materialManager)
{
    mLogicalFileSystem = logicalFileSystem;
    mTextureManager = textureManager;
    mMaterialManager = materialManager;
}

void Quad::ImGuiSystem::Draw()
{

    D3DRender::D3DRenderSystem *renderSystem =
        static_cast<D3DRender::D3DRenderSystem *>(Render::IRenderSystem::GetInstance());

    ID3D12GraphicsCommandList *commandList = renderSystem->GetCommandList().Get();
    ID3D12DescriptorHeap *heaps[] = {m_ImGuiSrvHeap};

    ID3D12CommandAllocator *commandAlloc = renderSystem->GetCommandAllocator().Get();
    commandList->Reset(commandAlloc, nullptr);

    D3DRender::D3DWindowRenderManager *d3dWindowRenderManager = D3DRender::D3DWindowRenderManager::GetInstance();
    std::shared_ptr<D3DRender::D3DWindowRenderData> windowRenderData =
        d3dWindowRenderManager->GetWindowRenderData(mHwnd);

    GRM::GRMPtr backBuffer = windowRenderData->GetBackBuffer(windowRenderData->GetCurrentBackBufferIndex());

    D3DGRM::D3DGpuResource *d3dBackBuffer = static_cast<D3DGRM::D3DGpuResource *>(backBuffer.getResource());

    D3DGRM::D3DDescriptorHandle backBufferDescriptorHandle;
    d3dBackBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, backBufferDescriptorHandle);
    commandList->OMSetRenderTargets(1, &backBufferDescriptorHandle.mCpuDescriptorHandle, false, nullptr);

    D3D12_VIEWPORT viewport = {
        0,
    };
    RECT clientRect;
    GetClientRect(mHwnd, &clientRect);

    viewport.Width = clientRect.right;
    viewport.Height = clientRect.bottom;

    commandList->RSSetViewports(1, &viewport);

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    ImGui::Begin("Texture Test");

    ImGui::Text("안녕하세요");

    ImGui::End();

    DrawEditorBuildInfo();
    DrawLogicalFileSystem();
    DrawSelectAsset();
    DrawCreationMaterialPanel();
    DrawSavePanel();

    ImGui::Render();

    commandList->SetDescriptorHeaps(1, heaps);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

    commandList->Close();
    ID3D12CommandList *commandLists[] = {commandList};
    mCommandQueue->ExecuteCommandLists(1, commandLists);
}

void Quad::ImGuiSystem::DrawEditorBuildInfo()
{

    ImGui::Begin("에디터 빌드 정보");

    ImGui::Text("엔진 버전 :  ");
    ImGui::SameLine();
    ImGui::Text(BUILD_GIT_TAG);

    ImGui::Text("플랫폼 : ");
    ImGui::SameLine();
    ImGui::Text(BUILD_PLATFORM);

    ImGui::Text("빌드 구성 : ");
    ImGui::SameLine();
    ImGui::Text(BUILD_CONFIG);

    ImGui::End();
}

void Quad::ImGuiSystem::DrawLogicalFileSystem()
{

    ImGui::Begin("논리적 파일시스템 입니다");

    ImGui::Text("현재 폴더 : ");
    ImGui::SameLine();
    ImGui::Text(mLogicalFileSystem->GetCurrentLogicalFolder()->GetName().c_str());

    QuadLF::LogicalFolder *rootFolder = mLogicalFileSystem->GetRootFolder();

    DrawLogicalFolder(rootFolder);

    ImGui::End();
}

void Quad::ImGuiSystem::DrawLogicalFolder(QuadLF::LogicalFolder *logicalFolder)
{
    using namespace QuadLF;
    if (ImGui::TreeNode(logicalFolder->GetName().c_str()))
    {

        for (auto childNode : logicalFolder->GetChildNodeList())
        {

            if (childNode->GetNodeType() == ELogicalNodeType::eFolder)
            {
                // 폴더
                DrawLogicalFolder(static_cast<LogicalFolder *>(childNode));
            }
            else
            {

                LogicalFile *logicalFile = static_cast<LogicalFile *>(childNode);

                CoreAsset::EAssetType assetType = logicalFile->GetAssetInfo().mAssetType;

                // 파일

                if (assetType == CoreAsset::EAssetType::eTexture)
                {

                    const std::string textureLine = "Texture :  " + childNode->GetName();

                    if (ImGui::Selectable(textureLine.c_str(), &AssetClick))
                    {
                        // 이 asset texture 의 정보 저장
                        assetInfo = logicalFile->GetAssetInfo();
                    }
                }
                else if (assetType == CoreAsset::EAssetType::eMaterial)
                {

                    const std::string materialText = "Material :  " + childNode->GetName();
                    if (ImGui::Selectable(materialText.c_str(), &AssetClick))
                    {
                        assetInfo = logicalFile->GetAssetInfo();
                    }
                }
            }
        }

        ImGui::TreePop();
    }
}

void Quad::ImGuiSystem::DrawSelectAsset()
{
    ImGui::Begin("SelectAsset");

    if (AssetClick)
    {

        switch (assetInfo.mAssetType)
        {

        case CoreAsset::EAssetType::eTexture:
        {
            CoreAsset::Texture *texture = mTextureManager->GetAsset(assetInfo.mAssetID);

            ImGui::Text("Texture ID : ");
            ImGui::SameLine();
            ImGui::Text(std::to_string((uint32_t)texture->GetID()).c_str());

            uint32_t width = texture->GetWidth();
            uint32_t height = texture->GetHeight();
            D3DGRM::D3DGpuResource *d3dResource = static_cast<D3DGRM::D3DGpuResource *>(texture->GetGpuResource());

            D3DGRM::D3DDescriptorHandle handle;
            d3dResource->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eSRV, handle);

            ImGui::Image((ImTextureID)handle.mGpuDescriptorHandle.ptr, ImVec2((float)width, (float)height));
        }
        break;
        case CoreAsset::EAssetType::eMaterial:

        {
            CoreAsset::Material *material = mMaterialManager->GetAsset(assetInfo.mAssetID);

            ImGui::Text("Material ID : ");
            ImGui::SameLine();
            ImGui::Text(std::to_string((uint32_t)material->GetID()).c_str());
        }
        break;
        }
    }

    ImGui::End();
}

void Quad::ImGuiSystem::DrawCreationMaterialPanel()
{
    ImGui::Begin("Material Creation Panel");

    ImGui::InputScalar("GpuMaterialID", ImGuiDataType_U32, &mCurrMaterialCreationContext.mGpuMaterialID);

    if (ImGui::Button("SetGpuMaterial"))
    {

        D3DRender::D3DMaterialManager *d3dMaterialManager =
            static_cast<D3DRender::D3DMaterialManager *>(D3DRender::D3DMaterialManager::GetInstance());

        D3DRender::D3DMaterialItem *gpuMaterialItem;

        gpuMaterialItem = d3dMaterialManager->GetMaterialItem(mCurrMaterialCreationContext.mGpuMaterialID);
        if (gpuMaterialItem != nullptr)
        {

            mSetGpuMaterialID = true;

            const Render::ShaderResourceInfoSet &shaderResourceInfoSet = gpuMaterialItem->mShaderResourceInfoSet;

            mCurrMaterialCreationContext.mTexResourceInfo.resize(
                shaderResourceInfoSet.mTextureShaderResourceInfoVector.size());

            for (int i = 0; i < mCurrMaterialCreationContext.mTexResourceInfo.size(); ++i)
            {
                mCurrMaterialCreationContext.mTexResourceInfo[i].first =
                    shaderResourceInfoSet.mTextureShaderResourceInfoVector[i].mName;
            }

            mCurrMaterialCreationContext.mSamplerResourceInfo.resize(
                shaderResourceInfoSet.mSamplerShaderResourceInfoVector.size());
        }
        else
        {
            mSetGpuMaterialID = false;
        }
    }

    if (mSetGpuMaterialID)
    {
        for (int i = 0; i < mCurrMaterialCreationContext.mTexResourceInfo.size(); ++i)
        {

            ImGui::Text(mCurrMaterialCreationContext.mTexResourceInfo[i].first.c_str());
            ImGui::SameLine();
            ImGui::InputScalar("ID : ", ImGuiDataType_U32, &mCurrMaterialCreationContext.mTexResourceInfo[i].second);
        }

        for (int i = 0; i < mCurrMaterialCreationContext.mSamplerResourceInfo.size(); ++i)
        {
            ImGui::InputScalar("Sampler ID : ", ImGuiDataType_U32,
                               &mCurrMaterialCreationContext.mSamplerResourceInfo[i]);
        }

        ImGui::InputText("MaterialName", mCurrMaterialCreationContext.mAssetMaterialName, 255);

        if (ImGui::Button("Create"))
        {

            // 수행
            // 머터리얼 생성
            // 설정
            //  결과
            CoreAsset::MaterialManager *materialManager = CoreAsset::MaterialManager::GetInstance();

            // 현재 가리키는 폴더기능필요
            // mLogicalFileSystem->get

            CoreAsset::Material *material = materialManager->CreateMaterial(
                mCurrMaterialCreationContext.mGpuMaterialID, mCurrMaterialCreationContext.mAssetMaterialName, "Asset");

            for (int i = 0; i < mCurrMaterialCreationContext.mTexResourceInfo.size(); ++i)
            {
                material->SetTextureResource(
                    i, mTextureManager->GetAsset(mCurrMaterialCreationContext.mTexResourceInfo[i].second));
            }

            for (int i = 0; i < mCurrMaterialCreationContext.mSamplerResourceInfo.size(); ++i)
            {
                material->SetSamplerResource(i, mCurrMaterialCreationContext.mSamplerResourceInfo[i]);
            }

            // 저장플래그 true
            material->SetDirty();

            // 논리적파일에 추가까지

            QuadLF::LogicalFileAssetInfo logicalFileAssetInfo;
            logicalFileAssetInfo.mAssetID = material->GetID();
            logicalFileAssetInfo.mAssetType = CoreAsset::EAssetType::eMaterial;
            logicalFileAssetInfo.mName = material->GetName();

            QuadLF::LogicalFile *materialLogicalFile = mLogicalFileSystem->MakeFile(
                logicalFileAssetInfo, material->GetName(), mLogicalFileSystem->GetRootFolder());

            // 에셋 메타데이터 추가
            CoreAsset::AssetMetaDataManager *assetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();

            CoreAsset::AssetMetaData assetMetaData;
            assetMetaData.mAssetID = material->GetID();
            assetMetaData.mAssetName = material->GetName();
            assetMetaData.mAssetType = CoreAsset::EAssetType::eMaterial;
            assetMetaData.mFilePath = materialLogicalFile->GetFullPath();
            assetMetaData.mKeepRawDataFlag = false;

            assetMetaDataManager->Register(assetMetaData);

            bMaterialCreationresult = true;
        }

        ImGui::Text("Result : ");
        ImGui::SameLine();

        if (bMaterialCreationresult == true)
        {
            ImGui::Text("Success");
        }
        else
        {
            ImGui::Text("Fail");
        }
    }

    ImGui::End();
}

void Quad::ImGuiSystem::DrawSavePanel()
{
    ImGui::Begin("프로젝트 저장 판넬");

    if (ImGui::Button("프로젝트 저장(현재 에셋만)"))
    {
        // 에셋 저장
        // wirte된것들에 한해서
        // asset manager- > save asset  //dirty flag가 켜져있는것들에대해서(아니면 save flag를 따로 둬야하나)

        EditorProjectManager *saver = EditorProjectManager::GetInstance();
        saver->SaveProject();
    }

    ImGui::End();
}
