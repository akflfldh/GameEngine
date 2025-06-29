#include "RenderSystem.h"
#include"Object/Camera/Camera.h"
#include"Object/Entity.h"
#include"GraphicCommand.h"
//#include"Text.h"
//#include"Character.h"
#include"Buffer/StructuredBuffer.h"
#include"InstancingTable.h"
#include"InstancingTableType.h"
#include"RenderPass.h"
#include"Shader/ShaderResource.h"
#include"Buffer/ConstantBuffer.h"
#include"Shader/ShaderResourceTexture.h"
#include"Shader/ShaderResourceSampler.h"
#include"Shader/ShaderResourceConstantBuffer.h"

#include"EffectTable.h"
#include"Object/Light/Light.h"
#include"System.h"
#include"Effect/RenderPassTwo.h"
#include"PassData.h"

#include"RenderPassSystem.h"
#include"Object/Line/LineBase.h"


#include<Component/ColliderBaseComponent.h>
#include"ResourceManager/TextureManager/TextureManager.h"

#include"ResourceManager/MeshManager.h"

#include"ResourceManager/EffectManager/EffectManager.h"
#include"ScissorRectComponent.h"

#include<Component/IMeshComponent.h>

namespace Quad
{


     std::unique_ptr<ModelSubMesh>RenderSystem:: mDefaultEntireRectModelSubMesh;
     RenderItem RenderSystem::mDefaultEntireRectRenderItem;
     Effect* RenderSystem::mRenderTargetToBackBufferEffect = nullptr;


     void RenderSystem::NotifyCreatingMapLayer(ESystemType systemType, D3D12_VIEWPORT viewportLocal, D3D12_VIEWPORT viewportGlobal)
     {

         //mapLayerRenderData를 생성 ,구축한다.


         std::vector<MapLayerRenderData>& mapLayerRenderDataVector = mMapLayerRenderDataTable[systemType];


         MapLayerRenderData mapLayerRenderData;

         //default rendertarget 생성
         RenderTargetTexture* renderTargetTexture = CreateDefaultRenderTargetTexture(viewportGlobal.Width, viewportGlobal.Height);

         mapLayerRenderData.SetDefaultRenderTarget(renderTargetTexture);

         //default depthstencil buffer 생성

         Texture* depthStencilBuffer = CreateDefaultDepthStencilBuffer(viewportGlobal.Width, viewportGlobal.Height);
         depthStencilBuffer->SetTextureType(ETextureType::eDepthStencilBuffer);


         mapLayerRenderData.SetDefaultDepthStencilBuffer(depthStencilBuffer);

         //view port 설정 
         mapLayerRenderData.SetViewport(viewportLocal);
         mapLayerRenderData.SetViewportGlobal(viewportGlobal);


         //default effect 설정


         const std::vector<std::string>& effectNameVector = mEffectNameVectorPerSystemTable[systemType];

         for (auto& effectName : effectNameVector)
         {
             Effect* effect = EffectManager::GetEffect(effectName);
            
             Effect*  copiedEffect=  new Effect(*effect);

             mapLayerRenderData.AddEffect(copiedEffect);
         }


         mapLayerRenderDataVector.push_back(std::move(mapLayerRenderData));




     }

     void RenderSystem::NotifyResizeMapLayer(ESystemType systemType, int mapLayerIndex, D3D12_VIEWPORT viewportLocal, D3D12_VIEWPORT viewportGlobal)
     {
         std::vector<MapLayerRenderData>& mapLayerRenderDataVector = mMapLayerRenderDataTable[systemType];

         mapLayerRenderDataVector[mapLayerIndex].OnResize(viewportGlobal.Width, viewportGlobal.Height);

     }

     bool RenderSystem::InitD3d()
    {
        mGraphicCommandObject->FlushCommandQueue();
        ThrowIfFailed(mGraphicCommandObject->ResetCommandList(nullptr));

        //ThrowIfFailed(mGraphicsCommandList->Reset(mCommandAllocator.Get(), nullptr));


        CreateSwapChain();
        //CreateDescriptorHeaps();
        CreateSwapChainRtv();
        CreateSwapChainDepthStencilBufferAndView();

        mClientViewPort.TopLeftX = 0;
        mClientViewPort.TopLeftY = 0;
        mClientViewPort.Width = (float)mClientWidth;
        mClientViewPort.Height = (float)mClientHeight;
        mClientViewPort.MinDepth = 0.0f;
        mClientViewPort.MaxDepth = 1.0f;
        //m_graphicscommandList->RSSetViewports(1, &m_clientViewPort);

        mScissorRect.left = 0;
        mScissorRect.right = (LONG)mClientWidth;
        mScissorRect.top = 0;
        mScissorRect.bottom = (LONG)mClientHeight;
        //m_graphicscommandList->RSSetScissorRects(1, &m_scissorRect);

        mBackgroundColor[0] = 0.3f;
        mBackgroundColor[1] = 0.2f;
        mBackgroundColor[2] = 0.4f;
        mBackgroundColor[3] = 1.0f;

        mDefaultMapLayerRenderTargetColor[0] = 0.0f;
        mDefaultMapLayerRenderTargetColor[0] = 0.0f;
        mDefaultMapLayerRenderTargetColor[0] = 0.0f;
        mDefaultMapLayerRenderTargetColor[0] = 0.0f;


        mGraphicCommandObject->ExecuteCommandList();

        //FlushCommandQueue();
        OnResize(mClientWidth, mClientHeight);
        //commandlist에서 reset을호출하기위해서는 닫혀있어야한다.
        return true;
    }



    void RenderSystem::CreateSwapChain()
    {
        mSwapchain.Reset();//이함수를 안전하게 재사용할수있다(스왑체인인터페이스를 다시만들수있다)

        DXGI_SWAP_CHAIN_DESC scDesc;
        DXGI_MODE_DESC bufferDesc;
        bufferDesc.Width = mClientWidth;
        bufferDesc.Height = mClientHeight;
        bufferDesc.RefreshRate.Numerator = 60;
        bufferDesc.RefreshRate.Denominator = 1;
        bufferDesc.Format = mBackBufferForamt;
        bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        scDesc.BufferDesc = bufferDesc;
        scDesc.SampleDesc.Count = 1;
        scDesc.SampleDesc.Quality = 0;
        scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.BufferCount = mSwapchainBufferCount;
        scDesc.OutputWindow = mHwnd;
        scDesc.Windowed = true;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;      //direct12에서는 flip만사용가능. 그리고 후면버퍼에서 멀티샘플링불가능
        scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        //여기서 서술하는 많은 스왑체인생성을위한 특성들은 지금 다 알필요가없고 책을 보고기본셋팅만유지한다.

        HRESULT hresult = mFactory->CreateSwapChain(mGraphicCommandObject->GetCommandQueue().Get(), &scDesc, mSwapchain.GetAddressOf());

        ThrowIfFailed(hresult);


    }

    void RenderSystem::CreateSwapChainDepthStencilBufferAndView()
    {
        D3D12_RESOURCE_DESC dsbDesc;
        dsbDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        dsbDesc.Alignment = 0;
        dsbDesc.Width = mClientWidth;
        dsbDesc.Height = mClientHeight;
        dsbDesc.DepthOrArraySize = 1;
        dsbDesc.MipLevels = 1;
        dsbDesc.Format = mSwapchainDepthStencilBufferFormat;
        dsbDesc.SampleDesc.Count = 1;
        dsbDesc.SampleDesc.Quality = 0;
        dsbDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        dsbDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        /*D3D12_HEAP_PROPERTIES dsbDefaultHeapPro;
        dsbDefaultHeapPro.Type = D3D12_HEAP_TYPE_DEFAULT;
        dsbDefaultHeapPro.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        dsbDefaultHeapPro.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        dsbDefaultHeapPro.CreationNodeMask = 0;
        dsbDefaultHeapPro.VisibleNodeMask = 0;*/

        CD3DX12_HEAP_PROPERTIES dsbDefaultHeapPro(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_CLEAR_VALUE depthClearValue;
        depthClearValue.Format = mSwapchainDepthStencilBufferFormat;
        depthClearValue.DepthStencil.Depth = 1.0f;
        depthClearValue.DepthStencil.Stencil = 0;


        ThrowIfFailed(mDevice->CreateCommittedResource(&dsbDefaultHeapPro, D3D12_HEAP_FLAG_NONE, &dsbDesc, D3D12_RESOURCE_STATE_COMMON
            , &depthClearValue, IID_PPV_ARGS(mSwapchainDepthStencilBufferResource.GetAddressOf())));






        D3D12_DEPTH_STENCIL_VIEW_DESC swapchainDsv;
        swapchainDsv.Format = mSwapchainDepthStencilBufferFormat;
        swapchainDsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        swapchainDsv.Flags = D3D12_DSV_FLAG_NONE;
        swapchainDsv.Texture2D.MipSlice = 0;


        if (mSwapChainDsvIndex != -1)
            mDescriptorHeapManagerMaster->ReleaseDsv(mSwapChainDsvIndex);

        mSwapChainDsvIndex = mDescriptorHeapManagerMaster->CreateDsv(mSwapchainDepthStencilBufferResource, &swapchainDsv);
        //CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(mSwapchainDsvHeap->GetCPUDescriptorHandleForHeapStart());
        //mDevice->CreateDepthStencilView(mSwapchainDepthStencilBuffer.Get(), &swapchainDsv, dsvHeapHandle);
        //nullptr지정해도 자원의 format과 dimension을상속하고 밉맵수준0에대한 뷰를 생성한다.
        //단 자원의 format이 구체적으로 정해진것이아니라면(typeless) 불가능 



        auto dsbTransition = CD3DX12_RESOURCE_BARRIER::Transition(mSwapchainDepthStencilBufferResource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        mGraphicCommandObject->GetGraphicsCommandList()->ResourceBarrier(1, &dsbTransition);



     //   mDefaultDepthStencilBuffer.SetViewIndex(mSwapChainDsvIndex);
    //   mDefaultDepthStencilBuffer.SetTextureResource(mSwapchainDepthStencilBufferResource);



    }


    void RenderSystem::CreateSwapChainRtv()
    {



        for (size_t i = 0; i < mSwapchainBufferCount; ++i)
        {
            if (mSwapChainBufferRtvIndex[i] != -1)
            {
                mDescriptorHeapManagerMaster->ReleaseRtv(mSwapChainBufferRtvIndex[i]);
            }
            ThrowIfFailed(mSwapchain->GetBuffer(i, IID_PPV_ARGS(&mSwapchainBuffer[i])));
            mSwapChainBufferRtvIndex[i] = mDescriptorHeapManagerMaster->CreateRtv(mSwapchainBuffer[i], nullptr);
        }
    }
    RenderTargetTexture* RenderSystem::CreateDefaultRenderTargetTexture(int width,int height)
    {
        
        RenderTargetTexture* renderTargetTexture = TextureManager::CreateRenderTargetTextureRandomName(width,height);
        if (renderTargetTexture == nullptr)
        {
            OutputDebugString(L"렌더시스템 디폴트 렌더타켓 생성실패\n");
            assert(0);
            //return nullptr;
        }

        renderTargetTexture->SetEngineContentItemFlag(true);
      return renderTargetTexture;




    }
    Texture* RenderSystem::CreateDefaultDepthStencilBuffer(int width, int height)
    {

        Texture* depthStencilBuffer = TextureManager::CreateDepthStencilBufferRandomName(width, height);

        if (depthStencilBuffer == nullptr)
        {
            OutputDebugString(L"렌더시스템 디폴트 깊이버퍼 생성실패\n");
            assert(0);
        }
        depthStencilBuffer->SetEngineContentItemFlag(true);
        return depthStencilBuffer;
    }

    void RenderSystem::CreateMapLayerDefaultRenderTargetTextureResource(RenderTargetTexture* renderTargetTexture,int width, int height)
    {
        if (renderTargetTexture == nullptr)
            return;


        TextureManager::CreateRenderTargetTextureResource(renderTargetTexture, width, height);
    }

    void RenderSystem::CreateMapLayerDefaultRenderTargetTextureResourceAll()
    {



        
        for (auto& mapLayerRenderDataVectorElement : mMapLayerRenderDataTable)
        {

            std::vector<MapLayerRenderData>& mapLayerRenderDataVector = mapLayerRenderDataVectorElement.second;

            for (auto& mapLayerRendeerDataElement : mapLayerRenderDataVector)
            {

                RenderTargetTexture* renderTargetTexture = mapLayerRendeerDataElement.GetDefaultRenderTarget();
                D3D12_VIEWPORT viewport =  mapLayerRendeerDataElement.GetViewportGlobal();

               
                CreateMapLayerDefaultRenderTargetTextureResource(renderTargetTexture,viewport.Width,viewport.Height);

            }


        }






    }

    void RenderSystem::CreateMapLayerDefaultDepthStencilBufferResource(Texture* depthStencilBuffer,int width, int height)
    {
        if (depthStencilBuffer == nullptr)
            return;


        TextureManager::CreateDepthStencilBufferResource(depthStencilBuffer, width, height);

    }

    void RenderSystem::CreateMapLayerDefaultDepthStencilBufferResourceAll()
    {

        for (auto& mapLayerRenderDataVectorElement : mMapLayerRenderDataTable)
        {

            std::vector<MapLayerRenderData>& mapLayerRenderDataVector = mapLayerRenderDataVectorElement.second;

            for (auto& mapLayerRendeerDataElement : mapLayerRenderDataVector)
            {

                Texture* depthStencilBuffer = mapLayerRendeerDataElement.GetDefaultDepthStencilBuffer();
                D3D12_VIEWPORT viewport = mapLayerRendeerDataElement.GetViewportGlobal();


                CreateMapLayerDefaultDepthStencilBufferResource(depthStencilBuffer, viewport.Width, viewport.Height);

            }


        }





    }


    void RenderSystem::ReleaseMapLayerDefaultRenderTargetTextureResource(RenderTargetTexture * renderTarget)
    {
        if (renderTarget == nullptr)
            return;


            mDescriptorHeapManagerMaster->ReleaseRtv(renderTarget->GetRendedrTargetViewIndex());
            mDescriptorHeapManagerMaster->ReleaseCbvSrvUav(renderTarget->GetViewIndex());



            renderTarget->GetResource()->Release();
            renderTarget->SetTextureResource(nullptr);


          
    }
    void RenderSystem::ReleaseMapLayerDefaultDepthStencilBufferResource(Texture* depthstencilBuffer)
    {
        if (depthstencilBuffer == nullptr)
            return;


        mDescriptorHeapManagerMaster->ReleaseDsv(depthstencilBuffer->GetViewIndex());

        depthstencilBuffer->GetResource()->Release();
        depthstencilBuffer->SetTextureResource(nullptr);


    }


    void RenderSystem::ReleaseMapLayerDefaultRenderTargetTextureResourceAll()
    {

        //system 별로

       //모든 mapLayer의 renderTargetTexture 제거 

        for (auto& mapLayerRenderDataVectorElement : mMapLayerRenderDataTable)
        {

            std::vector<MapLayerRenderData>& mapLayerRenderDataVector = mapLayerRenderDataVectorElement.second;

            for (auto& mapLayerRendeerDataElement : mapLayerRenderDataVector)
            {

                RenderTargetTexture* renderTargetTexture = mapLayerRendeerDataElement.GetDefaultRenderTarget();

                ReleaseMapLayerDefaultRenderTargetTextureResource(renderTargetTexture);

            }


        }
    }

    void RenderSystem::ReleaseMapLayerDefaultDepthStencilBufferResourceAll()
    {

        for (auto& mapLayerRenderDataVectorElement : mMapLayerRenderDataTable)
        {

            std::vector<MapLayerRenderData>& mapLayerRenderDataVector = mapLayerRenderDataVectorElement.second;

            for (auto& mapLayerRendeerDataElement : mapLayerRenderDataVector)
            {

                Texture* depthStencilBuffer = mapLayerRendeerDataElement.GetDefaultDepthStencilBuffer();

               
                ReleaseMapLayerDefaultDepthStencilBufferResource(depthStencilBuffer);

            }


        }



    }

   



    ID3D12Resource* RenderSystem::GetCurrentBackBuffer() const
    {
        return mSwapchainBuffer[mCurrBackBufferIndex].Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE RenderSystem::GetCurrentBackBufferView() const
    {

        /*   return CD3DX12_CPU_DESCRIPTOR_HANDLE(mSwapchainRtvHeap->GetCPUDescriptorHandleForHeapStart(),
               mCurrBackBufferIndex, mRtvdescriptorSize);*/

        return mDescriptorHeapManagerMaster->GetRtvCpuDescriptorHandle(mSwapChainBufferRtvIndex[mCurrBackBufferIndex]);

    }

    D3D12_CPU_DESCRIPTOR_HANDLE RenderSystem::GetDepthStencilBufferView() const
    {
        return mDescriptorHeapManagerMaster->GetDsvCpuDescriptorHandle(mSwapChainDsvIndex);
    }


    void RenderSystem::OnResize(UINT clientWidth, UINT clientHeight)
    {

        mClientWidth = clientWidth;
        mClientHeight = clientHeight;


        mGraphicCommandObject->FlushCommandQueue();
        mGraphicCommandObject->ResetCommandAllocator();
        mGraphicCommandObject->ResetCommandList(nullptr);


        for (int i = 0; i < mSwapchainBufferCount; ++i)
            mSwapchainBuffer[i].Reset();
        mSwapchainDepthStencilBufferResource.Reset();

        ThrowIfFailed(mSwapchain->ResizeBuffers(2, mClientWidth, mClientHeight, mBackBufferForamt, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

        mCurrBackBufferIndex = 0;

        CreateSwapChainRtv();
        CreateSwapChainDepthStencilBufferAndView();

        
        //
        // 
        // 
        
        //OnResizeMapLayerTexture();


        ////mapLayer들의 렌더타켓,깊이버퍼 제거
        //ReleaseMapLayerDefaultRenderTargetTextureResourceAll();
        //ReleaseMapLayerDefaultDepthStencilBufferResourceAll();

        ////새로운 크기로 렌더타켓,깊이버퍼 생성
        //CreateMapLayerDefaultRenderTargetTextureResourceAll();
        //CreateMapLayerDefaultDepthStencilBufferResourceAll();
    




        //effect resize
        OnResizeMapLayerRenderData(clientWidth, clientHeight);










        mGraphicCommandObject->ExecuteCommandList();
        mGraphicCommandObject->FlushCommandQueue();
     

        mCurrBackBufferIndex = 0;

        mClientViewPort.TopLeftY = 0.0;
        mClientViewPort.TopLeftX = 0.0;
        mClientViewPort.Width = (float)mClientWidth;
        mClientViewPort.Height = (float)mClientHeight;
        mClientViewPort.MinDepth = 0.0f;
        mClientViewPort.MaxDepth = 1.0f;

        mScissorRect.left = 0;
        mScissorRect.right = mClientWidth;
        mScissorRect.top = 0;
        mScissorRect.bottom = mClientHeight;

        OutputDebugString(L"swap\n");




       // SetDefaultRenderTargetAndDepthStencilToRenderPassSystem();




    }

    void RenderSystem::SetBackgroundColor(float r, float g, float b, float a)
    {
        mBackgroundColor[0] = r;
        mBackgroundColor[1] = g;
        mBackgroundColor[2] = b;
        mBackgroundColor[3] = a;
    }

    float RenderSystem::GetAspectRatio() const
    {
        return static_cast<float>(mClientWidth) / mClientHeight;
    }






































    void RenderSystem::PreUpdate()
    {


        for (std::unordered_map<ESystemType, std::vector<LightItem*>>::iterator it = mLightItemVectorPerSystem.begin();
            it != mLightItemVectorPerSystem.end(); ++it)
        {
            std::vector<LightItem*>& lightItemVector = it->second;
                 
            for (int lightIndex = 0; lightIndex < lightItemVector.size(); ++lightIndex)
            {
                mLightItemPoolAllocator->ReleaseInstance(lightItemVector[lightIndex]);
            }
            lightItemVector.clear();
        }

  /*      LightItem* lightItemTest = mLightItemPoolAllocator->GetInstance();
        static Light light(L"");
        lightItemTest->mLight = &light;
        mLightItemVector.push_back(lightItemTest);*/

        ResetResource();




    }

    void RenderSystem::Update()
    {

        UploadEntityData();


   


    }

    void RenderSystem::Draw()
    {


        //stencil buffer에대한 draw가 필요하다.

      //  BeforeDraw(true);

      //  mMainRenderPassSystem.Draw(true);
      ////  mUiRenderPassSystem.Draw(true);
      //  mWindowLayoutRenderPassSystem.Draw(true);
      //  mWindowDockingRenderPassSystem.Draw(true);



        //후면버퍼에대한 설정 ,렌더링시작을위한 기본셋팅 
        BeforeDraw(false);

        mMainRenderPassSystem.Draw(false);
      //  mUiRenderPassSystem.Draw(false);
        mWindowLayoutRenderPassSystem.Draw(false);
        mWindowDockingRenderPassSystem.Draw(false);


        AfterDraw();

    }

    /*void RenderSystem::DrawEffect(const Effect * effect , const std::vector<RenderItem *> & renderItemVector,
        const RenderSettingItem& renderSettingItem, ESystemType systemType)
    {
       const std::vector<RenderPassTwo*> & renderPassVector = effect->GetStaticRenderPassVector();

       for (int i = 0; i < renderPassVector.size(); ++i)
       {
           DrawRenderPass(effect ,renderPassVector[i], renderItemVector, renderSettingItem,systemType);
       }

    }*/

    //void RenderSystem::DrawRenderPass(const Effect* effect, const RenderPassTwo* renderPass, const std::vector<RenderItem*>& renderItemVector,
    //    const RenderSettingItem& renderSettingItem , ESystemType systemType)
    //{


    //    //mGraphicCommandObject->ResetCommandList(renderPass->GetPipelineState().Get());
    //      
    //    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicCommandObject->GetGraphicsCommandList();
    //    graphicsCommandList->SetPipelineState(renderPass->GetPipelineState().Get());

    //    graphicsCommandList->SetGraphicsRootSignature(renderPass->GetRootSignature().Get());
    //    graphicsCommandList->RSSetViewports(1, &renderSettingItem.mViewPort);
    //    //resource bind 
    //    const std::vector<ShaderResource*>& shaderResourceVector = renderPass->GetShaderResourceVector();

    //    //pass resource per pass 
    //    BindShaderResource(shaderResourceVector, nullptr ,0,systemType,false);
    //    

    //    for (int i = 0; i < renderItemVector.size(); ++i) 
    //    {
    //        //bind resource per object pass
    //        BindShaderResource(shaderResourceVector, renderItemVector[i], i,systemType,true);
    //       
    //        //stencil 
    //        graphicsCommandList->OMSetStencilRef(renderItemVector[i]->mStencilRefValue);

    //        //vertex , index bind 
    //       // Entity* entity = (Entity*)renderItemVector[i]->mObject;
    //        Mesh* mesh = renderItemVector[i]->mSubMesh.mMesh;
    //        D3D12_VERTEX_BUFFER_VIEW vbv= mesh->GetVertexBufferView();
    //        D3D12_INDEX_BUFFER_VIEW ibv = mesh->GetIndexBufferView();

    //        graphicsCommandList->IASetVertexBuffers(0, 1, &vbv);
    //        graphicsCommandList->IASetIndexBuffer(&ibv);

    //        UINT instanceCount = renderItemVector[i]->mInstanceCount;
    //        UINT indexStart = renderItemVector[i]->mSubMesh.mIndexRange.first;
    //        UINT indexEnd = renderItemVector[i]->mSubMesh.mIndexRange.second;
    //        UINT vertexStart = renderItemVector[i]->mSubMesh.mVertexOffset;
    //        UINT indexCount = indexEnd - indexStart;
    //        //draw 
    //        D3D_PRIMITIVE_TOPOLOGY topologyType = renderPass->GetPrimitiveTopologyType();
    //        graphicsCommandList->IASetPrimitiveTopology(topologyType);
    //        graphicsCommandList->DrawIndexedInstanced(indexCount, instanceCount, indexStart, vertexStart, 0);
    //    
    //    }


    //}


    //void RenderSystem::BindShaderResource(const std::vector<ShaderResource *>& shaderResourceVector,RenderItem* renderItem,
    //    int elementIndex ,ESystemType systemType, bool objectOrPassFlag)
    //{

    //    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList  = mGraphicCommandObject->GetGraphicsCommandList();
    //    //D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
    //    //완성이되면 공통이부분은 switch문에서 빼낼것.

    //    //shaderResource순서대로 rootsignature에 바인딩된다.
    //    for (int shaderResourceIndex = 0; shaderResourceIndex < shaderResourceVector.size(); ++shaderResourceIndex)
    //    {
    //        ShaderResource* currentShaderResource = shaderResourceVector[shaderResourceIndex];
    //        if (currentShaderResource->GetObjectOrPassFlag() != objectOrPassFlag)
    //            continue;


    //        switch (currentShaderResource->GetShaderResourceType())
    //        {
    //        case EShaderResourceType::eConstantBuffer:
    //        {
    //            ShaderResourceConstantBuffer* shaderResourceConstantBuffer = static_cast<ShaderResourceConstantBuffer*>(currentShaderResource);

    //            UINT viewIndexOffset = shaderResourceConstantBuffer->GetSystemViewIndexOffset(systemType);

    //            ViewIndex viewIndex = shaderResourceConstantBuffer->GetViewIndex(viewIndexOffset + elementIndex);

    //            D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle = mDescriptorHeapManagerMaster->GetCbvSrvUavGpuDescriptorHandle(viewIndex);

    //            graphicsCommandList->SetGraphicsRootDescriptorTable(shaderResourceIndex, gpuDescriptorHandle);

    //        }
    //        break;
    //        case EShaderResourceType::eTexture:
    //            //텍스처도 렌더아이템별일수도있고 스카이박스배경일수도있을것이다. 확장될것
    //        {
    //            ShaderResourceTexture* shaderResourceTexture = static_cast<ShaderResourceTexture*>(currentShaderResource);

    //            const std::wstring& shaderResourceTextureName = shaderResourceTexture->GetName();

    //            if (shaderResourceTextureName == L"gDiffuseMap")
    //            {
    //                Texture* texture = renderItem->mSubMesh.mMateiral->GetDiffuseMap();
    //                ViewIndex viewIndex = texture->GetViewIndex();
    //                D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle =  mDescriptorHeapManagerMaster->GetCbvSrvUavGpuDescriptorHandle(viewIndex);

    //                graphicsCommandList->SetGraphicsRootDescriptorTable(shaderResourceIndex, gpuDescriptorHandle);

    //            }
    //            else if (shaderResourceTextureName == L"gNormalMap")
    //            {




    //            }

    //        }
    //            break;
    //        case EShaderResourceType::eSampler:
    //        {

    //            ShaderResourceSampler* shaderResourceSampler = static_cast<ShaderResourceSampler*>(currentShaderResource);

    //            ViewIndex viewIndex =  shaderResourceSampler->GetViewIndex();

    //            D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle = mDescriptorHeapManagerMaster->GetSpvGpuDescriptorHandle(viewIndex);

    //            graphicsCommandList->SetGraphicsRootDescriptorTable(shaderResourceIndex, gpuDescriptorHandle);
    //        }
    //            break;
    //        }





    //    }

    // 
    //    return;

    //}


    void RenderSystem::SetRenderItemToPassSystem(const std::vector<RenderItem*>& renderItemVector , ESystemType systemType, int mapLayerIndex )
    {
      
        std::unordered_map<std::wstring,std::vector<RenderItem * >> *renderItemVectorPerEffect =nullptr;
        RenderPassSystem* renderPassSystem = nullptr;
        switch (systemType)
        {
        case ESystemType::eMainSystem:
            renderPassSystem = &mMainRenderPassSystem;
            break;
        case ESystemType::eWindowLayoutSystem:
            renderPassSystem = &mWindowLayoutRenderPassSystem;
            break;
        case ESystemType::eDockingSystem:
            renderPassSystem = &mWindowDockingRenderPassSystem;
            break;

        }

        SetRenderItemToPassSystem_FixedPass(renderPassSystem, renderItemVector,mapLayerIndex,systemType);
        SetRenderItemToPassSystem_CommonPass(renderPassSystem, renderItemVector,mapLayerIndex,systemType);



    }

    void RenderSystem::SetMapLayerVector(const std::vector<MapLayer>& mapLayerVector, ESystemType systemType)
    {

        //엔티티에대한 렌더item을 만들어서 렌더passSystem에 추가하면된다.  

        //기존 setentityvector랑 크게 다른게없다.


        RenderPassSystem* renderPassSystem = nullptr;

  
        switch (systemType)
        {
        case ESystemType::eMainSystem:
        {
            renderPassSystem = &mMainRenderPassSystem;
        }
        break;
        case ESystemType::eWindowLayoutSystem:
          
            renderPassSystem = &mWindowLayoutRenderPassSystem;
            break;
        case ESystemType::eDockingSystem:
            renderPassSystem = &mWindowDockingRenderPassSystem;
            break;

        }

        SetMapLayerViewportRenderData(mapLayerVector, systemType);
        renderPassSystem->SetMapLayerVector(mapLayerVector,mMapLayerRenderDataTable[systemType]);



        for (int mapLayerIndex = 0; mapLayerIndex < mapLayerVector.size(); ++mapLayerIndex)
        {
            const std::vector<Object*>& objectVector = mapLayerVector[mapLayerIndex].mRenderObjectVector;
            std::vector<RenderItem*> renderItemVector;
            //공통의 renderItem list 생성 
            CreateRenderItem(objectVector, renderItemVector);

            //SetRenderItemToPassSystem(renderItemVector,systemType,mapLayerIndex);
            SetRenderItemToPassSystem(renderItemVector, systemType, mapLayerIndex);
        }










    }

    void RenderSystem::SetMapLayerViewportRenderData(const std::vector<MapLayer>& mapLayerVector, ESystemType systemType)
    {
       std::vector<MapLayerRenderData>& mapLayerRenderDataVector = mMapLayerRenderDataTable[systemType];

        for (int i = 0; i < mapLayerVector.size(); ++i)
        {
            mapLayerRenderDataVector[i].SetViewport(mapLayerVector[i].mViewPort);
            mapLayerRenderDataVector[i].SetViewportGlobal(mapLayerVector[i].mViewPortGlobal);
        }

    }

 
    void RenderSystem::UploadEntityData()
    {

        //system 별 upload하는순서도 중요 고정해야한다. viewIndexOffset때문에
      

        UploadEntityDataPerSystem(ESystemType::eMainSystem);

       // UploadEntityDataPerSystem(ESystemType::eUiSystem);

        UploadEntityDataPerSystem(ESystemType::eWindowLayoutSystem);

        UploadEntityDataPerSystem(ESystemType::eDockingSystem);




    }

    void RenderSystem::UploadEntityDataPerSystem(ESystemType systemType)
    {

        //passData를 구축
        PassData passData;
        const std::vector<LightItem*>& mLightItemVector = mLightItemVectorPerSystem[systemType];

        for (int lightIndex = 0; lightIndex < mLightItemVector.size(); ++lightIndex)
        {
            switch (mLightItemVector[lightIndex]->mLight->GetLightType())
            {
             case  ELightType::eDirect:
                 passData.mDirectionalLightItemVector.push_back(mLightItemVector[lightIndex]);
                 break;

             case ELightType::ePoint:
                 passData.mPointSpotLightItemVector.push_back(mLightItemVector[lightIndex]);
                 break;
             case ELightType::eSpot:
                 passData.mSpotLightItemVector.push_back(mLightItemVector[lightIndex]);
                 break;
           }
        }

        RenderSettingItem* renderSettingItem = nullptr;


        switch (systemType)
        {
        case ESystemType::eMainSystem:

            renderSettingItem = &mRenderSettingItem;
            passData.mCamera = renderSettingItem->mCamera;

            mMainRenderPassSystem.UploadData(passData);
           

          //  mGamePlayRenderPassSystem.SetRenderSettingItem(renderSettingItem);
            break;

        //case ESystemType::eUiSystem:

        //    renderSettingItem = &mRenderUiSettingItem;
        //    passData.mCamera = renderSettingItem->mCamera;
        //    mUiRenderPassSystem.UploadData(passData);
        //   // mUiRenderPassSystem.SetRenderSettingItem(renderSettingItem);
        //    break;

        case ESystemType::eWindowLayoutSystem:
            renderSettingItem = &mRenderWindowLayoutSettingItem;
            passData.mCamera = renderSettingItem->mCamera;
            mWindowLayoutRenderPassSystem.UploadData(passData);
           // mWindowLayoutRenderPassSystem.SetRenderSettingItem(renderSettingItem);
            break;
        case ESystemType::eDockingSystem:
            renderSettingItem = &mRenderWindowDockingSettingItem;
            passData.mCamera = renderSettingItem->mCamera;
            mWindowDockingRenderPassSystem.UploadData(passData);
         //   mWindowDockingRenderPassSystem.SetRenderSettingItem(renderSettingItem);
            break;

        }


    }

 

    void RenderSystem::ResetResource()
    {


        mMainRenderPassSystem.Reset();

     //   mUiRenderPassSystem.Reset();

        mWindowLayoutRenderPassSystem.Reset();

        mWindowDockingRenderPassSystem.Reset();



        for (int i = 0; i < mRenderItemVector.size(); ++i)
        {
            mRenderItemPoolAllocator->ReleaseInstance(mRenderItemVector[i]);
        }

        mRenderItemVector.clear();

        


        for (auto& mapLayerRenderDataVectorElement : mMapLayerRenderDataTable)
        {
            std::vector<MapLayerRenderData>& mapLayerRenderDataVector = mapLayerRenderDataVectorElement.second;
            for (auto& mapLayerRenderData : mapLayerRenderDataVector)
            {
                mapLayerRenderData.ResetEffectResource(); // reset effec resource()

            }
        }


        //typedef std::unordered_map<std::string, Effect*> effectTableUnMap;
        //for (std::unordered_map<ESystemType, std::vector<effectTableUnMap>>::iterator it = mEffectTablePerSystem.begin(); it != mEffectTablePerSystem.end(); ++it)
        //{

        //    std::vector<effectTableUnMap>   & effectTableVector = it->second;


        //    for (auto& effectTableUnMapElement : effectTableVector)
        //    {

        //        for (auto& effectElement : effectTableUnMapElement)
        //        {
        //            Effect* effect = effectElement.second;

        //            //reseteffectrerouce도 effect내에서 처리해야하 확실히 분리가되지
        //            ResetEffectResource(effect);

        //        }


        //    }





           /* for (std::unordered_map<std::string, Effect*>::iterator effectIterator = effectTable.begin();
                effectIterator != effectTable.end();  ++effectIterator)
            {
                Effect* effect = effectIterator->second;
                
                ResetEffectResource(effect);

            }*/

        }



    

    void RenderSystem::SetRenderMapItem(RenderMapItem& renderMapItem)
    {
        mRenderMapItem = renderMapItem;
    }

    void RenderSystem::SetRenderSettingItem(RenderSettingItem& renderSettingItem, ESystemType systemType)
    {


        switch (systemType)
        {
        case ESystemType::eMainSystem:
            mRenderSettingItem = renderSettingItem;
            break;
     /*   case ESystemType::eUiSystem:
            mRenderUiSettingItem = renderSettingItem;
            break;*/
        case ESystemType::eWindowLayoutSystem:
            mRenderWindowLayoutSettingItem = renderSettingItem;
            break;
        case ESystemType::eDockingSystem:
            mRenderWindowDockingSettingItem = renderSettingItem;
            break;

        }


    }

    void RenderSystem::SetRenderUiSettingItem(RenderSettingItem& renderUiSettingItem)
    {
        //mRenderUiSettingItem = renderUiSettingItem;
    }

    void RenderSystem::BeforeDraw(bool preStencilDrawFlag)
    {

        if (!mGraphicCommandObject->GetCloseState())
        {
            mGraphicCommandObject->ExecuteCommandList();
        }


        mGraphicCommandObject->FlushCommandQueue();

        //gpu가 명령들을 다실행했음을 보장해야한다.->동기화 필요->펜스이용
        mGraphicCommandObject->ResetCommandAllocator();
        mGraphicCommandObject->ResetCommandList(nullptr);
    
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicCommandObject->GetGraphicsCommandList();



        //기본 후면버퍼, 깊이버퍼 상태변경,설정 등등
        if (preStencilDrawFlag == false)
        {
            CD3DX12_RESOURCE_BARRIER swapChainStateTansition = CD3DX12_RESOURCE_BARRIER::Transition(mSwapchainBuffer[mCurrBackBufferIndex].Get(),
                D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

            graphicsCommandList->ResourceBarrier(1, &swapChainStateTansition);


        }

        //system들의 default renderTarget,depth  buffer도 적절한 상태로 설정
        //clear

        for (auto& mapLayerRenderDataVectorElement : mMapLayerRenderDataTable)
        {
            std::vector<MapLayerRenderData >& mapLayerRenderDataVector = mapLayerRenderDataVectorElement.second;

            for (auto& mapLayerRenderData : mapLayerRenderDataVector)
            {
                RenderTargetTexture * defaultMpaLayerRenderTarget =  mapLayerRenderData.GetDefaultRenderTarget();
                Texture* defaultMapLayerDepthStencilBuffer = mapLayerRenderData.GetDefaultDepthStencilBuffer();

                if (defaultMpaLayerRenderTarget != nullptr)
                {

                    CD3DX12_RESOURCE_BARRIER renderTargetStateTansition = CD3DX12_RESOURCE_BARRIER::Transition(defaultMpaLayerRenderTarget->GetResource().Get(),
                        D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);

                    graphicsCommandList->ResourceBarrier(1, &renderTargetStateTansition);


                   auto rtv = mDescriptorHeapManagerMaster->GetRtvCpuDescriptorHandle(defaultMpaLayerRenderTarget->GetRendedrTargetViewIndex());

                   auto dsv = mDescriptorHeapManagerMaster->GetDsvCpuDescriptorHandle(defaultMapLayerDepthStencilBuffer->GetViewIndex());


                    graphicsCommandList->ClearRenderTargetView(rtv, mDefaultMapLayerRenderTargetColor, 0, nullptr);
                    //    graphicsCommandList->ClearRenderTargetView(rtvHandle, mBackgroundColor, 0, nullptr);
                    graphicsCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
                }



            }


        }






      //  D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =mDescriptorHeapManagerMaster->GetRtvCpuDescriptorHandle(mSwapChainBufferRtvIndex[mCurrBackBufferIndex]).


        auto rtv= GetCurrentBackBufferView();
        auto dsv = GetDepthStencilBufferView();



       // mMainRenderPassSystem.SetCurrentRenderTargetDepthStencilView(rtvCpuHandle, dsvCpuHandle);
      //  mUiRenderPassSystem.SetCurrentRenderTargetDepthStencilView(rtvCpuHandle, dsvCpuHandle);
     //   mWindowLayoutRenderPassSystem.SetCurrentRenderTargetDepthStencilView(rtvCpuHandle, dsvCpuHandle);
      //  mWindowDockingRenderPassSystem.SetCurrentRenderTargetDepthStencilView(rtvCpuHandle, dsvCpuHandle);




    
        if (preStencilDrawFlag)
        {
            graphicsCommandList->OMSetRenderTargets(0, nullptr
                , true, &dsv);
            graphicsCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
 
        }
        else
        {
            graphicsCommandList->OMSetRenderTargets(1, &rtv
                , true, &dsv);
            graphicsCommandList->ClearRenderTargetView(rtv, mBackgroundColor, 0, nullptr);
        //    graphicsCommandList->ClearRenderTargetView(rtvHandle, mBackgroundColor, 0, nullptr);
            graphicsCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH , 1.0f, 0, 0, nullptr);

        }
        graphicsCommandList->RSSetScissorRects(1, &mScissorRect);
       
 
        //release가 안됬어.
 

        ID3D12DescriptorHeap* descriptorHeaps[2] = { mDescriptorHeapManagerMaster->GetCbvSrvUavDescriptorHeap().Get(),
        mDescriptorHeapManagerMaster->GetSpvDescriptorHeap().Get() };
        graphicsCommandList->SetDescriptorHeaps(2, descriptorHeaps);




    }


    void RenderSystem::AfterDraw()
    {
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicCommandObject->GetGraphicsCommandList();


      

        //모든 시스템들의 각 mapLayer들의 default renderTarget을 srv바인딩하고 viewport에 따라 적절하게 후면버퍼로 렌더링한다.
        


        //이것은 좀 특별한 렌더시스템만의 draw 

        D3D12_CPU_DESCRIPTOR_HANDLE backBufferCpuDescriptorHandle = GetCurrentBackBufferView();
        D3D12_CPU_DESCRIPTOR_HANDLE depthBufferCpuDescriptorHandle = GetDepthStencilBufferView();
        graphicsCommandList->OMSetRenderTargets(1, &backBufferCpuDescriptorHandle, true, &depthBufferCpuDescriptorHandle);




        RenderPassTwo  *toBackBufferRenderPass = mRenderTargetToBackBufferEffect->GetStaticRenderPassVector()[0];


        graphicsCommandList->SetPipelineState(toBackBufferRenderPass->GetPipelineState().Get());
        //root signature binding
        graphicsCommandList->SetGraphicsRootSignature(toBackBufferRenderPass->GetRootSignature().Get());

        //descriptor heap binding
        graphicsCommandList->SetDescriptorHeaps(1, mDescriptorHeapManagerMaster->GetCbvSrvUavDescriptorHeap().GetAddressOf());



        for (auto& mapLayerRenderDataVectorElement : mMapLayerRenderDataTable)
        {
            if (mapLayerRenderDataVectorElement.first == ESystemType::eDockingSystem)
                continue;
                

            for (const auto& mapLayerRenderDataElement : mapLayerRenderDataVectorElement.second)
            {

                D3D12_VIEWPORT viewportLocal  = mapLayerRenderDataElement.GetViewport();
                 RenderTargetTexture * defaultRenderTarget=      mapLayerRenderDataElement.GetDefaultRenderTarget();
                //graphic pipeline binding

                 //viewport설정,(전체)
                 
               // graphicsCommandList->RSSetViewports(1, &mClientViewPort);
                 D3D12_VIEWPORT viewport = mapLayerRenderDataElement.GetViewportGlobal();
                graphicsCommandList->RSSetViewports(1, &viewport);

               
                //pass cosntant binding , texture ,
                float passData[4] = { viewportLocal.TopLeftY + viewportLocal.Width / 2 ,
                                        viewportLocal.TopLeftY + viewportLocal.Height / 2,
                                    viewportLocal.Width ,viewportLocal.Height };
                graphicsCommandList->SetGraphicsRoot32BitConstants(0, 4, passData, 0);



                //srv,constantbuffer view 사용을 위해서 렌더패스를 사용할것인가?

                D3D12_GPU_DESCRIPTOR_HANDLE defaultRenderTargetGpuDescriptorHandle = mDescriptorHeapManagerMaster->GetCbvSrvUavGpuDescriptorHandle(defaultRenderTarget->GetViewIndex());


                graphicsCommandList->SetGraphicsRootDescriptorTable(1, defaultRenderTargetGpuDescriptorHandle);


                //아니면 shaderResoruce 바인딩을 그 root costant binding으로 처리할까?
                
                //많은 값들이 들어가지않을거같으니 쉽게 처리가능할듯.



                //rect vertex ,index binding 
                StaticMesh* rectMesh = static_cast<StaticMesh*>(mDefaultEntireRectModelSubMesh->mMesh);

                D3D12_VERTEX_BUFFER_VIEW vertexBufferView = rectMesh->GetVertexBufferView();
                graphicsCommandList->IASetVertexBuffers(0,1,&vertexBufferView);


                D3D12_INDEX_BUFFER_VIEW  indexBufferView = rectMesh->GetIndexBufferView();
                graphicsCommandList->IASetIndexBuffer(&indexBufferView);


                graphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



                //draw
                size_t indexCount = mDefaultEntireRectModelSubMesh->mIndexRange.second - mDefaultEntireRectModelSubMesh->mIndexRange.first;
                graphicsCommandList->DrawIndexedInstanced(indexCount, 1, 0,0, 0);
                
            }


        }





















        //스왑체인 제시 상태로 전환
        CD3DX12_RESOURCE_BARRIER swapChainStateTansition = CD3DX12_RESOURCE_BARRIER::Transition(mSwapchainBuffer[mCurrBackBufferIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        graphicsCommandList->ResourceBarrier(1, &swapChainStateTansition);




    
       mGraphicCommandObject->ExecuteCommandList();
       mGraphicCommandObject->FlushCommandQueue();

        mSwapchain->Present(0, 0);
        mCurrBackBufferIndex = (mCurrBackBufferIndex + 1) % mSwapchainBufferCount;





    }

    void RenderSystem::ResetEffectResource(Effect* effect)
    {

        const std::vector<RenderPassTwo*>& staticRenderPassVector = effect->GetStaticRenderPassVector();
        const std::vector<RenderPassTwo*>& skinningRenderPassVector = effect->GetSkinningRenderPassVector();
        ResetRenderPassResource(staticRenderPassVector);
        ResetRenderPassResource(skinningRenderPassVector);






    }

    void RenderSystem::AddEffect(const Effect& effect,ESystemType systemType)
    {
     
        // 해당effect를 10개 복사 

        //mapLayer도 고려해야한다.
        //일단 미루고


        typedef std::unordered_map<std::string, Effect*> effectTableUnMap;

        std::vector<MapLayerRenderData>& mapLayerRenderDataVector = mMapLayerRenderDataTable[systemType];

        if (mapLayerRenderDataVector.size() != 0)
        {

            Effect* pEffect = mapLayerRenderDataVector[0].GetEffect(effect.GetName());
            if (pEffect != nullptr)
                return;
        }


        switch (systemType)
        {
        case ESystemType::eMainSystem:
        {
            //10개의 mapLayer를 고려해서 각각 effect생성
            for (int i = 0; i < 10; ++i)
            {

                Effect* pEffect = new Effect(effect);

                mapLayerRenderDataVector[i].AddEffect(pEffect);

              //  effectTablePerMapLayer[i].emplace(effect.GetName(), pEffect);
                CreateEffectTexture(pEffect);
            }



        }
        break;
        case ESystemType::eWindowLayoutSystem:
        case ESystemType::eDockingSystem:

            //하나의 mapLayer만 있다.
        {
            Effect* pEffect = new Effect(effect);
            mapLayerRenderDataVector[0].AddEffect(pEffect);
            //effectTablePerMapLayer[0].emplace(effect.GetName(), pEffect);
            CreateEffectTexture(pEffect);


        }
        break;
        }


            return;

    }

    Effect* RenderSystem::GetEffect(const std::string& name, Quad::ESystemType systemType, int mapLayerIndex) const
    {

        const auto & mapLayerRenderDataVectorIt = mMapLayerRenderDataTable.find(systemType);
       
        if (mapLayerRenderDataVectorIt == mMapLayerRenderDataTable.cend())
            return nullptr;


        Effect * effect =   mapLayerRenderDataVectorIt->second[mapLayerIndex].GetEffect(name);

        return effect;
    }

  

    void RenderSystem::SetColliderWorldRenderState(bool state)
    {
        mColliderWorldRenderState = state;
    
    }

    bool RenderSystem::GetColliderWorldRenderState() const
    {
        return mColliderWorldRenderState;
    }

    void RenderSystem::SetColliderDrawFlag(bool flag)
    {
        mColliderDrawFlag = flag;
    }

    bool RenderSystem::GetColliderDrawFlag() const
    {
        return mColliderDrawFlag;
    }

    void RenderSystem::SetRenderState(bool state)
    {
        mRenderState = state;
    }

    bool RenderSystem::GetRenderState() const
    {
        return mRenderState;
    }

    void RenderSystem::ResetRenderPassResource(const std::vector<RenderPassTwo*>& renderPassVector)
    {
        for (int renderPassIndex = 0; renderPassIndex < renderPassVector.size(); ++renderPassIndex)
        {

            const std::vector<ShaderResource*>& shaderResourceVector = renderPassVector[renderPassIndex]->GetShaderResourceVector();

            for (int shaderResourceIndex = 0; shaderResourceIndex < shaderResourceVector.size(); ++shaderResourceIndex)
            {
                if (shaderResourceVector[shaderResourceIndex]->GetShaderResourceType() == EShaderResourceType::eConstantBuffer)
                {

                    ShaderResourceConstantBuffer* shaderResourceCosntantBuffer = (ShaderResourceConstantBuffer*)
                        shaderResourceVector[shaderResourceIndex];
                    shaderResourceCosntantBuffer->Reset();


                }
            }
        }
    }

    void RenderSystem::OnResizeMapLayerRenderData(UINT clientWidth, UINT clientHeight)
    {
        typedef std::unordered_map<std::string, Effect*>  effectTableUnMap;

        
        //system별로 mapLayer별로
        //effect별로 가지는 custom 렌더타켓, 텍스처 들의 크기를 조정해야한다 
        //기존 텍스처 제거, 새로 생성

        //for (auto& systemRenderDataElement : mMapLayerRenderDataTable)
        //{

        //    //각각의 system별의 clientWidth,clientHeight 가 필요

        //    std::vector<MapLayerRenderData>& mapLayerRederDataVector = systemRenderDataElement.second;

        //    for (auto& renderDataElement : mapLayerRederDataVector)
        //    {
        //        renderDataElement.OnResize(clientWidth ,clientHeight);
        //    }

        //}

    }


    void RenderSystem::InitializeDefaultRenderItem()
    {
        //여러 시스템이 동시초기화가 된다면 call_once 를 사용하자
        static bool initOnce = false;

        if (initOnce == true)
            return;


        MeshManager* meshManager = MeshManager::GetInstance();

        Mesh* defaultEntireRectMesh = meshManager->GetMesh("DefaultEntireRect3");

        SubMesh defaultEntireRectSubMesh = defaultEntireRectMesh->GetSubMeshVector()[0];

        mDefaultEntireRectModelSubMesh = std::make_unique<Quad::ModelSubMesh>();

       // ModelSubMesh* defaultEntireRectModelSubMesh = new ModelSubMesh;
        mDefaultEntireRectModelSubMesh->mIndexRange = defaultEntireRectSubMesh.mIndexRange;
        mDefaultEntireRectModelSubMesh->mMesh = defaultEntireRectMesh;
        mDefaultEntireRectModelSubMesh->mVertexNum = defaultEntireRectSubMesh.mVertexNum;
        mDefaultEntireRectModelSubMesh->mVertexOffset = defaultEntireRectSubMesh.mVertexOffset;
       

      

        
        //mDefaultEntireRectRenderItem.mObjectType = EObjectType::eEntity;
        //mDefaultEntireRectRenderItem.mIndexBufferUsageFlag = 0;
        mDefaultEntireRectRenderItem.mInstanceCount = 1;
        mDefaultEntireRectRenderItem.mSubMeshIndex = 0;
        //mDefaultEntireRectRenderItem.mSubMesh = mDefaultEntireRectModelSubMesh.get();
       


        initOnce = true;
        
    }

    void RenderSystem::SetRenderItemToPassSystem_FixedPass(RenderPassSystem* renderPassSystem,
        const std::vector<RenderItem*>& renderItemVector, int mapLayerIndex, ESystemType systemType)
    {



    }

    //고유하게 submesh들이 가지는 effect들에대해 패스구축
    void RenderSystem::SetRenderItemToPassSystem_CommonPass(RenderPassSystem* renderPassSystem,
        const std::vector<RenderItem*>& renderItemVector, int mapLayerIndex ,ESystemType systemType)
    {
        
        //effect-pass 별로 renderItem 목록들을 유지한다.

        const MapLayerRenderData & mapLayerRenderData = mMapLayerRenderDataTable[systemType][mapLayerIndex];

        for (auto renderItemElement : renderItemVector)
        {
            IMeshComponent* meshComponent = renderItemElement->mMeshComponent;
            const std::vector<ModelSubMesh>& subMeshVector = meshComponent->GetSubMesh();
            int subMeshIndex = renderItemElement->mSubMeshIndex;

        
            const std::string & effectName= subMeshVector[subMeshIndex].mModelMaterial.GetEffectName();
            Effect * effect = mapLayerRenderData.GetEffect(effectName);
                           
            for (auto renderPassElement : effect->GetStaticRenderPassVector())
            {
                renderPassSystem->AddRenderItem(renderPassElement, renderItemElement, mapLayerIndex);
            }

        }
        


        












        //for (size_t objectIndex = 0; objectIndex < objectVector.size(); ++objectIndex)
        //{
        //    Object* object = objectVector[objectIndex];


        //   // Model * objectModel =  object->GetModel();
        //  // objectModel->GetMeshComponent()







        //}


























        //for (int i = 0; i < entityVector.size(); ++i)
        //{



        //    // Entity* entity = dynamic_cast<Entity*>(entityVector[i]);
        //    Light* light = dynamic_cast<Light*>(entityVector[i]);
        //    LineBase* lineBase = dynamic_cast<LineBase*>(entityVector[i]);




        //    Model* model = entityVector[i]->GetModel();
        //    MeshComponent* meshComponent = static_cast<MeshComponent*>(model->GetComponent(EComponentType::eMeshComponent));
        //    AnimationComponent* animationComponent = static_cast<AnimationComponent*>(model->GetAnimationComponent());


        //    if (meshComponent != nullptr)
        //    {



        //        const std::vector<ModelSubMesh>& subMeshVector = meshComponent->GetSubMesh();


        //        for (int subMeshIndex = 0; subMeshIndex < subMeshVector.size(); ++subMeshIndex)
        //        {
        //            std::string effectName = subMeshVector[subMeshIndex].mModelMaterial.GetEffectName();


        //            EMeshType meshType = meshComponent->GetMesh()->GetMeshType();
        //            if (effectName == "TextCharacter.effect")
        //            {
        //                int a = 2;
        //            }

        //            Effect* effect = GetEffect(effectName, systemType, mapLayerIndex);

        //            RenderItem* renderItem = mRenderItemPoolAllocator->GetInstance();       //풀 allocator사용할것.
        //            renderItem->mSceneElement = entityVector[i];// entity;
        //            renderItem->mObjectType = EObjectType::eEntity;
        //            renderItem->mSubMesh = &subMeshVector[subMeshIndex];
        //            renderItem->mStencilRefValue = entityVector[i]->GetStencilRefValue();
        //            SetScissorRectUsage(*renderItem, entityVector[i]);



        //            Object* mObject;
        //            renderItem->mInstanceCount = 1;
        //            // renderItem->mStencilRefValue = entity->GetStencilRefValue();

        //            mRenderItemVector.push_back(renderItem);

        //            if (meshType == EMeshType::eStaticMesh || animationComponent == nullptr || !animationComponent->GetAnimationComponentState())
        //            {
        //                //effectName += L"_Static";
        //                const std::vector<RenderPassTwo*>& renderPassVector = effect->GetStaticRenderPassVector();


        //                for (int renderPassIndex = 0; renderPassIndex < renderPassVector.size(); ++renderPassIndex)
        //                {

        //                    ERenderPassDrawType renderPassDrawType = renderPassVector[renderPassIndex]->GetRenderPassDrawType();
        //                    if (renderPassDrawType == ERenderPassDrawType::eObjectDraw)
        //                    {
        //                        renderPassSystem->AddRenderItem(renderPassVector[renderPassIndex], renderItem, mapLayerIndex);
        //                    }
        //                    else if (renderPassDrawType == ERenderPassDrawType::eEntirePostDraw)
        //                    {

        //                        renderPassSystem->AddRenderItem(renderPassVector[renderPassIndex], &mDefaultEntireRectRenderItem, mapLayerIndex);
        //                    }




        //                }


        //            }
        //            else if (meshType == EMeshType::eSknningMesh)
        //            {
        //                //effectName += L"Skinning";
        //                const std::vector<RenderPassTwo*>& renderPassVector = effect->GetSkinningRenderPassVector();

        //                for (int renderPassIndex = 0; renderPassIndex < renderPassVector.size(); ++renderPassIndex)
        //                {
        //                    ERenderPassDrawType renderPassDrawType = renderPassVector[renderPassIndex]->GetRenderPassDrawType();
        //                    if (renderPassDrawType == ERenderPassDrawType::eObjectDraw)
        //                    {
        //                        renderPassSystem->AddRenderItem(renderPassVector[renderPassIndex], renderItem, mapLayerIndex);
        //                    }
        //                    else if (renderPassDrawType == ERenderPassDrawType::eEntirePostDraw)
        //                    {

        //                        renderPassSystem->AddRenderItem(renderPassVector[renderPassIndex], &mDefaultEntireRectRenderItem, mapLayerIndex);
        //                    }
        //                }

        //            }


        //        }

        //    }


        //    if (GetColliderDrawFlag())
        //    {
        //        ColliderComponent* colliderComponent = static_cast<ColliderComponent*>(model->GetComponent(EComponentType::eColliderComponent));

        //        if (colliderComponent != nullptr)
        //        {
        //            Collider* collider = colliderComponent->GetCollider();

        //            if (systemType == ESystemType::eMainSystem && GetColliderWorldRenderState())
        //            {
        //                //collider draw




        //             //   Model& colliderModel = collider->GetModel();
        //                const std::vector<ModelSubMesh>& colliderSubMeshVector = collider->GetMeshComponent()->GetSubMesh();
        //                for (int subMeshIndex = 0; subMeshIndex < colliderSubMeshVector.size(); ++subMeshIndex)
        //                {
        //                    const std::string& effectName = colliderSubMeshVector[subMeshIndex].mModelMaterial.GetEffectName();
        //                    Effect* effect = GetEffect(effectName, systemType, mapLayerIndex);

        //                    RenderItem* renderItem = mRenderItemPoolAllocator->GetInstance();       //풀 allocator사용할것.
        //                    renderItem->mSceneElement = collider;
        //                    renderItem->mObjectType = EObjectType::eCollider;
        //                    renderItem->mSubMesh = &colliderSubMeshVector[subMeshIndex];

        //                    /*    renderItem->mSubMesh.mID = colliderSubMeshVector[i].mID;
        //                        renderItem->mSubMesh.mIndexRange = colliderSubMeshVector[i].mIndexRange;
        //                        renderItem->mSubMesh.mMaterialPointer = colliderSubMeshVector[i].mMateiral;
        //                        renderItem->mSubMesh.mMesh = colliderSubMeshVector[i].mMesh;
        //                        renderItem->mSubMesh.mName = colliderSubMeshVector[i].mName;
        //                        renderItem->mSubMesh.mVertexNum = colliderSubMeshVector[i].mVertexNum;
        //                        renderItem->mSubMesh.mVertexOffset = colliderSubMeshVector[i].mVertexOffset;*/



        //                        //renderItem->mSubMesh = &colliderSubMeshVector[i];
        //                    renderItem->mInstanceCount = 1;
        //                    renderItem->mStencilRefValue = 0; //collider->GetStencilRefValue();
        //                    mRenderItemVector.push_back(renderItem);

        //                    const std::vector<RenderPassTwo*>& renderPassVector = effect->GetStaticRenderPassVector();

        //                    for (int renderPassIndex = 0; renderPassIndex < renderPassVector.size(); ++renderPassIndex)
        //                    {
        //                        ERenderPassDrawType renderPassDrawType = renderPassVector[renderPassIndex]->GetRenderPassDrawType();
        //                        if (renderPassDrawType == ERenderPassDrawType::eObjectDraw)
        //                        {
        //                            renderPassSystem->AddRenderItem(renderPassVector[renderPassIndex], renderItem, mapLayerIndex);
        //                        }
        //                        else if (renderPassDrawType == ERenderPassDrawType::eEntirePostDraw)
        //                        {

        //                            renderPassSystem->AddRenderItem(renderPassVector[renderPassIndex], &mDefaultEntireRectRenderItem, mapLayerIndex);
        //                        }
        //                    }

        //                }

        //            }
        //        }
        //    }










        //    else if (lineBase != nullptr)
        //    {
        //        RenderItem* renderItem = mRenderItemPoolAllocator->GetInstance();
        //        renderItem->mSceneElement = lineBase;
        //        if (lineBase->GetName() == "Spline1")
        //        {
        //            int a = 2;
        //        }

        //        Effect* effect = GetEffect(lineBase->GetEffectName(), systemType, mapLayerIndex);

        //        const std::vector<RenderPassTwo*>& renderPassVector = effect->GetStaticRenderPassVector();
        //        renderItem->mIndexBufferUsageFlag = false;
        //        renderItem->mObjectType = EObjectType::eLine;
        //        renderItem->mSubMesh = lineBase->GetModelSubMesh();
        //        renderItem->mInstanceCount = 1;
        //        SetScissorRectUsage(*renderItem, entityVector[i]);


        //        for (int renderPassIndex = 0; renderPassIndex < renderPassVector.size(); ++renderPassIndex)
        //        {
        //            ERenderPassDrawType renderPassDrawType = renderPassVector[renderPassIndex]->GetRenderPassDrawType();
        //            if (renderPassDrawType == ERenderPassDrawType::eObjectDraw)
        //            {
        //                renderPassSystem->AddRenderItem(renderPassVector[renderPassIndex], renderItem, mapLayerIndex);
        //            }
        //            else if (renderPassDrawType == ERenderPassDrawType::eEntirePostDraw)
        //            {

        //                renderPassSystem->AddRenderItem(renderPassVector[renderPassIndex], &mDefaultEntireRectRenderItem, mapLayerIndex);
        //            }
        //        }
        //        mRenderItemVector.push_back(renderItem);


        //    }
        //    else if (light != nullptr)
        //    {
        //        LightItem* lightItem = mLightItemPoolAllocator->GetInstance();
        //        lightItem->mLight = light;
        //        mLightItemVectorPerSystem[systemType].push_back(lightItem);
        //    }

        //}













    }

    void RenderSystem::SetScissorRectUsage(RenderItem& renderItem, Object* object)
    {
        ScissorRectComponent* scissorRectComponent = object->GetComponent< ScissorRectComponent>("ScissorRectComponent");
   //     RECT scissorRect;
        if (scissorRectComponent)
        {
            renderItem.mScissorRectUsageFlag = true;
            renderItem.mScissorRect = scissorRectComponent->GetRect();
        }
        else
        {
            renderItem.mScissorRectUsageFlag = false;
        }
    }

    void RenderSystem::ReigsterDefaultEffect(ESystemType systemType, const std::string& effectName)
    {

        std::vector<std::string> & effectNameVector = mEffectNameVectorPerSystemTable[systemType];
        std::vector<std::string> ::iterator it = std::find(effectNameVector.begin(), effectNameVector.end(), effectName);

        if (it != effectNameVector.end())
            return;


        effectNameVector.push_back(effectName);

        return;

    }

    RenderItem* RenderSystem::FindInstancingRenderItem(const std::vector<RenderItem*>& renderItemVector, const Mesh* mesh, int subMeshIndex)
    {
        
     /*   for (int i = 0; i < renderItemVector.size(); ++i)
        {
            if ((renderItemVector[i]->mSubMesh.mMesh == mesh) && (renderItemVector[i]->mSubMeshIndex == subMeshIndex))
            {
                return renderItemVector[i];
            }
        }*/


        return nullptr;
    }

    void RenderSystem::CreateEffectTexture(Effect* effect)
    {
        Effect* pEffect = effect;
        unsigned int customTextureNum = pEffect->GetCustomTextureNum();
        if (customTextureNum == 0)
            return;



        const std::vector<std::pair<float, float>>& customTextureResizeRateVector = pEffect->GetCustomTextureResizeRateVector();

        const std::vector<ETextureType >& customTextureTypeVector = pEffect->GetCustomTextureTypeVector();




        //effect texture 생성

        std::vector<unsigned long long > customTextureIDVector(customTextureResizeRateVector.size());
        std::vector<Texture* > customTextureVector(customTextureResizeRateVector.size());

        for (size_t i = 0; i < customTextureNum; ++i)
        {
            float widthRate = customTextureResizeRateVector[i].first;
            float HeightRate = customTextureResizeRateVector[i].second;

            int textureWidth = mClientWidth * widthRate;            //mClientWidth  
            int textureHeight = mClientWidth * HeightRate;          //mClientHeight  들은 mapLayer별로 설정이되어야한다.즉 mapLayer의 viewport에서 width,height를 가져와야한다.

            switch (customTextureTypeVector[i])
            {
            case ETextureType::eDefaultTexture:

                //srv만 가지는 default texture
                customTextureVector[i] = TextureManager::CreateTextureRandomName(textureWidth, textureHeight);
                break;

            case ETextureType::eRenderTargetTexture:

                //rtv 까지 가지는 render target texture
                customTextureVector[i] = TextureManager::CreateRenderTargetTextureRandomName(textureWidth, textureHeight);

                break;

            }




            customTextureIDVector[i] = customTextureVector[i]->GetUniqueID();

        }



        pEffect->SetCustomTextureVector(std::move(customTextureVector));

        /*     const std::vector<RenderPassTwo*>& staticRenderPassVector =  pEffect->GetStaticRenderPassVector();



             for (auto renderPassElement : staticRenderPassVector)
             {
                 int renderTaregetLocalID =renderPassElement->GetCustomRenderTargetLocalID();
                const std::vector<RenderPassCommand *>   renderPassElement->GetPreRenderPassCommandVector();

             }

             pEffect->SetCustomRenderTargetTextureIDVector(std::move(customRenderTargetTextureIDVector));
             */






    }

    void RenderSystem::InitMapLayerRenderDataTable()
    {

       // mMapLayerRenderDataTable[ESystemType::eMainSystem].resize(10);
     //   mMapLayerRenderDataTable[ESystemType::eDockingSystem].resize(2);
     //   mMapLayerRenderDataTable[ESystemType::eWindowLayoutSystem].resize(2);




    }

    void RenderSystem::CreateRenderItem(const std::vector<Object*>& objectVector, std::vector<RenderItem*>& oRenderItemVector)
    {


    }



    RenderSystem::RenderSystem()
        :m4xmsaaQuality(1)
    {
        for (size_t i = 0; i < mSwapchainBufferCount; ++i)
            mSwapChainBufferRtvIndex[i] = -1;

        mSwapChainDsvIndex = -1;

    }


    RenderSystem::~RenderSystem()
    {
    
    }

    void RenderSystem::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
        Microsoft::WRL::ComPtr<IDXGIFactory4> factory,
        GraphicCommand* graphicCommandObject,
        HWND hWnd, UINT clientWidth, UINT clientHeight,
        DescriptorHeapManagerMaster* descriptorHeapManagerMaster, int windowID)
    {
        mDevice = device;
        mFactory = factory;
        mGraphicCommandObject = graphicCommandObject;
        mDescriptorHeapManagerMaster = descriptorHeapManagerMaster;
        mHwnd = hWnd;
        mClientWidth = clientWidth;
        mClientHeight = clientHeight;
        mWindowType = windowID;
        InitD3d();            
   //     CreateDefaultRenderTargetTexture();

        mMainRenderPassSystem.Initialize(graphicCommandObject,descriptorHeapManagerMaster);
       // mUiRenderPassSystem.Initialize(graphicCommandObject, descriptorHeapManagerMaster);
        mWindowLayoutRenderPassSystem.Initialize(graphicCommandObject, descriptorHeapManagerMaster);
        mWindowDockingRenderPassSystem.Initialize(graphicCommandObject, descriptorHeapManagerMaster);

    //    mMainRenderPassSystem.SetDefaultRenderTargetAndDepthStencil(&mDefaultRenderTargetTexture, &mDefaultDepthStencilBuffer);

        InitializeDefaultRenderItem();

        InitMapLayerRenderDataTable();

        mRenderItemPoolAllocator = new DestructingPoolAllocator<RenderItem>(10000);
        mRenderItemPoolAllocator->Initialize();

        mLightItemPoolAllocator = new DestructingPoolAllocator<LightItem>(1000);
        mLightItemPoolAllocator->Initialize();

    
        if(mRenderTargetToBackBufferEffect ==nullptr)
             mRenderTargetToBackBufferEffect = EffectManager::GetEffect("RenderTargetToBackBufferEffect");





    }


}







