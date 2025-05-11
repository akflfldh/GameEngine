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

//#include"TextCharacterEntity.h"
//#include"TextCharacterPanelEntity.h"

#include"Component/ColliderComponent.h"


namespace Quad
{
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
            , &depthClearValue, IID_PPV_ARGS(mSwapchainDepthStencilBuffer.GetAddressOf())));


        D3D12_DEPTH_STENCIL_VIEW_DESC swapchainDsv;
        swapchainDsv.Format = mSwapchainDepthStencilBufferFormat;
        swapchainDsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        swapchainDsv.Flags = D3D12_DSV_FLAG_NONE;
        swapchainDsv.Texture2D.MipSlice = 0;


        if (mSwapChainDsvIndex != -1)
            mDescriptorHeapManagerMaster->ReleaseDsv(mSwapChainDsvIndex);

        mSwapChainDsvIndex = mDescriptorHeapManagerMaster->CreateDsv(mSwapchainDepthStencilBuffer, &swapchainDsv);
        //CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(mSwapchainDsvHeap->GetCPUDescriptorHandleForHeapStart());
        //mDevice->CreateDepthStencilView(mSwapchainDepthStencilBuffer.Get(), &swapchainDsv, dsvHeapHandle);
        //nullptr지정해도 자원의 format과 dimension을상속하고 밉맵수준0에대한 뷰를 생성한다.
        //단 자원의 format이 구체적으로 정해진것이아니라면(typeless) 불가능 



        auto dsbTransition = CD3DX12_RESOURCE_BARRIER::Transition(mSwapchainDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        mGraphicCommandObject->GetGraphicsCommandList()->ResourceBarrier(1, &dsbTransition);



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
    //
    //void RenderSystem::CreateSwapchainDsv()
    //{
    //    //D3D12_RESOURCE_DESC dsbDesc;
    //    //dsbDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    //    //dsbDesc.Alignment = 0;
    //    //dsbDesc.Width = mClientWidth;
    //    //dsbDesc.Height = mClientHeight;
    //    //dsbDesc.DepthOrArraySize = 1;
    //    //dsbDesc.MipLevels = 1;
    //    //dsbDesc.Format = mSwapchainDepthStencilBufferFormat;
    //    //dsbDesc.SampleDesc.Count = 1;
    //    //dsbDesc.SampleDesc.Quality = 0;
    //    //dsbDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    //    //dsbDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    //
    //    ///*D3D12_HEAP_PROPERTIES dsbDefaultHeapPro;
    //    //dsbDefaultHeapPro.Type = D3D12_HEAP_TYPE_DEFAULT;
    //    //dsbDefaultHeapPro.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    //    //dsbDefaultHeapPro.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    //    //dsbDefaultHeapPro.CreationNodeMask = 0;
    //    //dsbDefaultHeapPro.VisibleNodeMask = 0;*/
    //
    //    //CD3DX12_HEAP_PROPERTIES dsbDefaultHeapPro(D3D12_HEAP_TYPE_DEFAULT);
    //    //D3D12_CLEAR_VALUE depthClearValue;
    //    //depthClearValue.Format = mSwapchainDepthStencilBufferFormat;
    //    //depthClearValue.DepthStencil.Depth = 1.0f;
    //    //depthClearValue.DepthStencil.Stencil = 0;
    //
    //
    //    //ThrowIfFailed(mDevice->CreateCommittedResource(&dsbDefaultHeapPro, D3D12_HEAP_FLAG_NONE, &dsbDesc, D3D12_RESOURCE_STATE_COMMON
    //    //    , &depthClearValue, IID_PPV_ARGS(&mSwapchainDepthStencilBuffer)));
    //
    //
    //    D3D12_DEPTH_STENCIL_VIEW_DESC swapchainDsv;
    //    swapchainDsv.Format = mSwapchainDepthStencilBufferFormat;
    //    swapchainDsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    //    swapchainDsv.Flags = D3D12_DSV_FLAG_NONE;
    //    swapchainDsv.Texture2D.MipSlice = 0;
    //
    //    mDescriptorHeapManagerMaster->CreateDsv(mSwapchainDepthStencilBuffer, &swapchainDsv);
    //    //CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(mSwapchainDsvHeap->GetCPUDescriptorHandleForHeapStart());
    //    //mDevice->CreateDepthStencilView(mSwapchainDepthStencilBuffer.Get(), &swapchainDsv, dsvHeapHandle);
    //    //nullptr지정해도 자원의 format과 dimension을상속하고 밉맵수준0에대한 뷰를 생성한다.
    //    //단 자원의 format이 구체적으로 정해진것이아니라면(typeless) 불가능 
    //
    //
    //
    //    auto dsbTransition = CD3DX12_RESOURCE_BARRIER::Transition(mSwapchainDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    //    mGraphicscommandList->ResourceBarrier(1, &dsbTransition);
    //
    //
    //
    //}

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
        mSwapchainDepthStencilBuffer.Reset();

        ThrowIfFailed(mSwapchain->ResizeBuffers(2, mClientWidth, mClientHeight, mBackBufferForamt, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

        mCurrBackBufferIndex = 0;

        CreateSwapChainRtv();
        CreateSwapChainDepthStencilBufferAndView();


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

        BeforeDraw(true);

        mMainRenderPassSystem.Draw(true);
      //  mUiRenderPassSystem.Draw(true);
        mWindowLayoutRenderPassSystem.Draw(true);
        mWindowDockingRenderPassSystem.Draw(true);



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


    void RenderSystem::AddEntity(Object* entity)
    {
        //mEntityVector.push_back(entity);
    }

    void RenderSystem::SetEntityVector(const std::vector<Object*>& entityVector , ESystemType systemType, int mapLayerIndex )
    {
      
        std::unordered_map<std::wstring,std::vector<RenderItem * >> *renderItemVectorPerEffect =nullptr;
        RenderPassSystem* renderPassSystem = nullptr;
        switch (systemType)
        {
        case ESystemType::eMainSystem:
            {
        //    renderItemVectorPerEffect = &mRenderItemPerEffectUnMap;
            renderPassSystem = &mMainRenderPassSystem;
            }
            break;
            
        //case ESystemType::eUiSystem:
        //   // renderItemVectorPerEffect = &mUiRenderItemPerEffectUnMap;
        //    renderPassSystem = &mUiRenderPassSystem;
        //    break;

        case ESystemType::eWindowLayoutSystem:
            //renderItemVectorPerEffect = &mWindowLayoutRenderItemPerEffectUnMap;
            renderPassSystem = &mWindowLayoutRenderPassSystem;
            break;
        case ESystemType::eDockingSystem:

           // renderItemVectorPerEffect = &mWindowDockingRenderItemPerEffectUnMap;
            renderPassSystem = &mWindowDockingRenderPassSystem;
            break;

        }

        //effect별로 renderitme을 분류해야한다.

        for (int i = 0; i < entityVector.size(); ++i)
        {

 

           // Entity* entity = dynamic_cast<Entity*>(entityVector[i]);
            Light* light = dynamic_cast<Light*> (entityVector[i]);
            LineBase* lineBase = dynamic_cast<LineBase*>(entityVector[i]);
           
            
            

            Model * model =  entityVector[i]->GetModel();
            MeshComponent * meshComponent = static_cast<MeshComponent*>( model->GetComponent(EComponentType::eMeshComponent));
            AnimationComponent* animationComponent = static_cast<AnimationComponent*>(model->GetAnimationComponent());
            
            if (meshComponent != nullptr)
            {
               
                

                    const std::vector<ModelSubMesh>& subMeshVector = meshComponent->GetSubMesh();


                    for (int subMeshIndex = 0; subMeshIndex < subMeshVector.size(); ++subMeshIndex)
                    {
                        std::string effectName = subMeshVector[subMeshIndex].mModelMaterial.GetEffectName();

         
                        EMeshType meshType = meshComponent->GetMesh()->GetMeshType();
                        Effect* effect = GetEffect(effectName, systemType);

                        RenderItem* renderItem = mRenderItemPoolAllocator->GetInstance();       //풀 allocator사용할것.
                        renderItem->mSceneElement = entityVector[i];// entity;
                        renderItem->mObjectType = EObjectType::eEntity;
                        renderItem->mSubMesh = &subMeshVector[subMeshIndex];
                        renderItem->mStencilRefValue = entityVector[i]->GetStencilRefValue();

                        Object* mObject;
                        renderItem->mInstanceCount = 1;
                       // renderItem->mStencilRefValue = entity->GetStencilRefValue();

                        mRenderItemVector.push_back(renderItem);

                        if (meshType == EMeshType::eStaticMesh || animationComponent == nullptr || !animationComponent->GetAnimationComponentState())
                        {
                            //effectName += L"_Static";
                            const std::vector<RenderPassTwo*>& renderPassVector = effect->GetStaticRenderPassVector();

                      
                            for (int renderPassIndex = 0; renderPassIndex < renderPassVector.size(); ++renderPassIndex)
                            {

                                renderPassSystem->AddRenderItem(renderPassVector[renderPassIndex], renderItem,mapLayerIndex);

                            }

                        }
                        else if (meshType == EMeshType::eSknningMesh)
                        {
                            //effectName += L"Skinning";
                            const std::vector<RenderPassTwo*>& renderPassVector = effect->GetSkinningRenderPassVector();

                            for (int renderPassIndex = 0; renderPassIndex < renderPassVector.size(); ++renderPassIndex)
                            {
                                renderPassSystem->AddRenderItem(renderPassVector[renderPassIndex], renderItem, mapLayerIndex);
                            }

                        }


                    }
            
            }
            

            if (GetColliderDrawFlag())
            {
                ColliderComponent* colliderComponent = static_cast<ColliderComponent*>(model->GetComponent(EComponentType::eColliderComponent));

                if (colliderComponent != nullptr)
                {
                    Collider* collider = colliderComponent->GetCollider();

                    if (systemType == ESystemType::eMainSystem && GetColliderWorldRenderState())
                    {
                        //collider draw




                     //   Model& colliderModel = collider->GetModel();
                        const std::vector<ModelSubMesh>& colliderSubMeshVector = collider->GetMeshComponent()->GetSubMesh();
                        for (int subMeshIndex = 0; subMeshIndex < colliderSubMeshVector.size(); ++subMeshIndex)
                        {
                            const std::string& effectName = colliderSubMeshVector[subMeshIndex].mModelMaterial.GetEffectName();
                            Effect* effect = GetEffect(effectName, systemType);

                            RenderItem* renderItem = mRenderItemPoolAllocator->GetInstance();       //풀 allocator사용할것.
                            renderItem->mSceneElement = collider;
                            renderItem->mObjectType = EObjectType::eCollider;
                            renderItem->mSubMesh = &colliderSubMeshVector[subMeshIndex];

                            /*    renderItem->mSubMesh.mID = colliderSubMeshVector[i].mID;
                                renderItem->mSubMesh.mIndexRange = colliderSubMeshVector[i].mIndexRange;
                                renderItem->mSubMesh.mMaterialPointer = colliderSubMeshVector[i].mMateiral;
                                renderItem->mSubMesh.mMesh = colliderSubMeshVector[i].mMesh;
                                renderItem->mSubMesh.mName = colliderSubMeshVector[i].mName;
                                renderItem->mSubMesh.mVertexNum = colliderSubMeshVector[i].mVertexNum;
                                renderItem->mSubMesh.mVertexOffset = colliderSubMeshVector[i].mVertexOffset;*/



                                //renderItem->mSubMesh = &colliderSubMeshVector[i];
                            renderItem->mInstanceCount = 1;
                            renderItem->mStencilRefValue = 0; //collider->GetStencilRefValue();
                            mRenderItemVector.push_back(renderItem);

                            const std::vector<RenderPassTwo*>& renderPassVector = effect->GetStaticRenderPassVector();

                            for (int renderPassIndex = 0; renderPassIndex < renderPassVector.size(); ++renderPassIndex)
                            {
                                renderPassSystem->AddRenderItem(renderPassVector[renderPassIndex], renderItem, mapLayerIndex);
                            }

                        }

                    }
                }
            }










            else if (lineBase != nullptr)
            {
                RenderItem* renderItem = mRenderItemPoolAllocator->GetInstance();
                renderItem->mSceneElement = lineBase;
                if (lineBase->GetName() == "Spline1")
                {
                    int a = 2;
                }

                Effect * effect =GetEffect(lineBase->GetEffectName(),systemType);

                const std::vector<RenderPassTwo*>& renderPassVector = effect->GetStaticRenderPassVector();
                renderItem->mIndexBufferUsageFlag = false;
                renderItem->mObjectType = EObjectType::eLine;
                renderItem->mSubMesh = lineBase->GetModelSubMesh();
                renderItem->mInstanceCount = 1;

                for (int i = 0; i < renderPassVector.size(); ++i)
                {
                    renderPassSystem->AddRenderItem(renderPassVector[i], renderItem, mapLayerIndex);
                }
                mRenderItemVector.push_back(renderItem);


            }
            else if (light != nullptr)
            {
                LightItem* lightItem = mLightItemPoolAllocator->GetInstance();
                lightItem->mLight = light;
                mLightItemVectorPerSystem[systemType].push_back(lightItem);
            }

        }


    }

    void RenderSystem::SetMapLayerVector(const std::vector<MapLayer>& mapLayerVector, ESystemType systemType)
    {

        //엔티티에대한 렌더item을 만들어서 렌더passSystem에 추가하면된다.  

        //기존 setentityvector랑 크게 다른게없다.

        //mapLayer을 정렬하자
        std::vector<MapLayer> alignedMapLayer = std::move(mapLayerVector);
        if (alignedMapLayer.size() == 2)
        {
            int a = 2;
        }
        std::sort(alignedMapLayer.begin(), alignedMapLayer.end(), [](const MapLayer& layerOne, const MapLayer& layerTwo) {
            return layerOne.mID > layerTwo.mID ? true : false; });



        RenderPassSystem* renderPassSystem = nullptr;
        switch (systemType)
        {
        case ESystemType::eMainSystem:
        {
            //    renderItemVectorPerEffect = &mRenderItemPerEffectUnMap;
            renderPassSystem = &mMainRenderPassSystem;
        }
        break;

        //case ESystemType::eUiSystem:
        //    // renderItemVectorPerEffect = &mUiRenderItemPerEffectUnMap;
        //    renderPassSystem = &mUiRenderPassSystem;
        //    break;

        case ESystemType::eWindowLayoutSystem:
            //renderItemVectorPerEffect = &mWindowLayoutRenderItemPerEffectUnMap;
            renderPassSystem = &mWindowLayoutRenderPassSystem;
            break;
        case ESystemType::eDockingSystem:

            // renderItemVectorPerEffect = &mWindowDockingRenderItemPerEffectUnMap;
            renderPassSystem = &mWindowDockingRenderPassSystem;
            break;

        }
        renderPassSystem->SetMapLayerVector(alignedMapLayer);


        for (int mapLayerIndex = 0; mapLayerIndex < alignedMapLayer.size(); ++mapLayerIndex)
        {
            const std::vector<Object*>& entityVector = alignedMapLayer[mapLayerIndex].mRenderObjectVector;
            SetEntityVector(entityVector, systemType, mapLayerIndex);
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
        for (std::unordered_map<ESystemType, std::unordered_map<std::string, Effect*>>::iterator it = mEffectTablePerSystem.begin(); it != mEffectTablePerSystem.end(); ++it)
        {

            std::unordered_map<std::string, Effect*>& effectTable = it->second;
            for (std::unordered_map<std::string, Effect*>::iterator effectIterator = effectTable.begin();
                effectIterator != effectTable.end();  ++effectIterator)
            {
                Effect* effect = effectIterator->second;
                
                ResetEffectResource(effect);

            }

         }



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

        

        if (preStencilDrawFlag == false)
        {
            CD3DX12_RESOURCE_BARRIER swapChainStateTansition = CD3DX12_RESOURCE_BARRIER::Transition(mSwapchainBuffer[mCurrBackBufferIndex].Get(),
                D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

            graphicsCommandList->ResourceBarrier(1, &swapChainStateTansition);

        }

        D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuHandle = mDescriptorHeapManagerMaster->GetRtvCpuDescriptorHandle(mSwapChainBufferRtvIndex[mCurrBackBufferIndex]);
        D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuHandle = mDescriptorHeapManagerMaster->GetDsvCpuDescriptorHandle(mSwapChainDsvIndex);

        
        mMainRenderPassSystem.SetCurrentRenderTargetDepthStencilView(rtvCpuHandle, dsvCpuHandle);
      //  mUiRenderPassSystem.SetCurrentRenderTargetDepthStencilView(rtvCpuHandle, dsvCpuHandle);
        mWindowLayoutRenderPassSystem.SetCurrentRenderTargetDepthStencilView(rtvCpuHandle, dsvCpuHandle);
        mWindowDockingRenderPassSystem.SetCurrentRenderTargetDepthStencilView(rtvCpuHandle, dsvCpuHandle);


        auto rtv = GetCurrentBackBufferView();
        auto dsv = GetDepthStencilBufferView();
        if (preStencilDrawFlag)
        {
            graphicsCommandList->OMSetRenderTargets(0, nullptr
                , true, &dsvCpuHandle);
            graphicsCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);



        }
        else
        {
            graphicsCommandList->OMSetRenderTargets(1, &rtvCpuHandle
                , true, &dsvCpuHandle);
            graphicsCommandList->ClearRenderTargetView(rtv, mBackgroundColor, 0, nullptr);
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
     
            std::unordered_map<std::string, Effect*>::iterator it = mEffectTablePerSystem[systemType].find(effect.GetName());
            if (it != mEffectTablePerSystem[systemType].end())
                return;

            Effect* pEffect = new Effect(effect);
            mEffectTablePerSystem[systemType].emplace(effect.GetName(), pEffect);
            return;
    }

    Effect* RenderSystem::GetEffect(const std::string& name, ESystemType systemType) const
    {
        const std::unordered_map<std::string, Effect*>& effectTable = mEffectTablePerSystem.find(systemType)->second;

        std::unordered_map<std::string, Effect*>::const_iterator it = effectTable.find(name);
        return it != effectTable.cend() ? it->second : nullptr;


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



    RenderSystem::RenderSystem()
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


        mMainRenderPassSystem.Initialize(graphicCommandObject,descriptorHeapManagerMaster);
       // mUiRenderPassSystem.Initialize(graphicCommandObject, descriptorHeapManagerMaster);
        mWindowLayoutRenderPassSystem.Initialize(graphicCommandObject, descriptorHeapManagerMaster);
        mWindowDockingRenderPassSystem.Initialize(graphicCommandObject, descriptorHeapManagerMaster);



        mRenderItemPoolAllocator = new DestructingPoolAllocator<RenderItem>(10000);
        mRenderItemPoolAllocator->Initialize();

        mLightItemPoolAllocator = new DestructingPoolAllocator<LightItem>(1000);
        mLightItemPoolAllocator->Initialize();


    }


}






