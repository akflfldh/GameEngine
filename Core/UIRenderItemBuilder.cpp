#include "Core/UIRenderItemBuilder.h"
#include "Core/GpuSamplerSystem.h"
#include <Core/ViewportController.h>
#include <CoreAsset/Material.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderSystem/IMaterialManager.h>
#include <RenderSystem/RenderType.h>
#include <UISystem/UICanvas.h>
#include <UISystem/UIElement.h>
#include <UISystem/UIManager.h>
#include <UISystem/UIRectTransform.h>
#include <UISystem/UIRenderableComponent.h>
#include <UISystem/UIResizeGizmoRenderableComponent.h>
#include <algorithm>
#include <assert.h>
#include <queue>
#include <stack>
#include <vector>

Quad::UIRenderItemBuilder *Quad::UIRenderItemBuilder::mInstance = nullptr;

Quad::UIRenderItemBuilder *Quad::UIRenderItemBuilder::GetInstance()
{
    if (mInstance == nullptr)
    {
        OutputDebugStringW(L"UI렌더아이템빌더 인스턴스가 생성되지않았습니다.\n");
        assert(0);
    }

    return mInstance;
}

Quad::UIRenderItemBuilder::UIRenderItemBuilder(Render::IRenderSystem *renderSystem, UI::UIManager *uiManager,
                                               GRM::IGpuResourceManager *gpuResourceManager)
    : mRenderSystem(renderSystem), mUiManager(uiManager), mGpuResourceManager(gpuResourceManager),
      mCurrVertexBufferOffset(0), mCurrIndexBufferOffset(0)
{

    if (mInstance != nullptr)
    {
        OutputDebugStringW(L"UI렌더아이템빌더 생성자 두번호출하였습니다. 한번만 호출해야합니다.\n");
        assert(0);
    }

    GRM::BufferDesc vertexBufferDesc;

    vertexBufferDesc.mBufferMemoryAccess = GRM::EBufferMemoryAccess::eCpuWriteOnly;
    vertexBufferDesc.mBufferUsage = GRM::EBufferUsage::eVertexBuffer;
    vertexBufferDesc.mElementDataNum = 4 * 10000;
    vertexBufferDesc.mElementDataSize = sizeof(UI::UIVertex);
    vertexBufferDesc.mBufferSize = 4 * 10000 * vertexBufferDesc.mElementDataSize;
    vertexBufferDesc.mData = nullptr;
    mVertexBufferGpu = mGpuResourceManager->CreateBuffer(vertexBufferDesc);

    GRM::BufferDesc indexBufferDesc;

    indexBufferDesc.mBufferMemoryAccess = GRM::EBufferMemoryAccess::eCpuWriteOnly;
    indexBufferDesc.mBufferUsage = GRM::EBufferUsage::eIndexBuffer;
    indexBufferDesc.mElementDataNum = 6 * 10000;
    indexBufferDesc.mElementDataSize = sizeof(uint32_t);
    indexBufferDesc.mBufferSize = 6 * 10000 * indexBufferDesc.mElementDataSize;
    indexBufferDesc.mData = nullptr;
    mIndexBufferGpu = mGpuResourceManager->CreateBuffer(indexBufferDesc);

    mInstance = this;
}

Quad::UIRenderItemBuilder::~UIRenderItemBuilder() {}

void Quad::UIRenderItemBuilder::SubmitUIElement(UI::UICanvas *canvas, Render::RenderChannelID renderChannelID,
                                                const Core::ViewportController &viewportController)
{

    if (canvas == nullptr)
        return;

    const std::vector<UI::UIElement *> &uiElementVector = canvas->GetChildUIElementAll();
    std::stack<UI::UIElement *> uiElementStack;

    for (auto uiElement : uiElementVector)
    {
        uiElementStack.push(uiElement);
    }

    Render::Viewport viewport = viewportController.GetViewport();

    std::vector<UIElementContext> &uiElementContextVector = mUIElementContextVectorPerRenderChannel[renderChannelID];
    while (!uiElementStack.empty())
    {

        UI::UIElement *uiElement = uiElementStack.top();
        uiElementStack.pop();

        size_t renderableComNum = uiElement->GetComponentsNum<UI::UIRenderableComponent>();
        if (renderableComNum != 0)
        {
            std::vector<UI::IUIComponent *> renderableComVector(renderableComNum);
            uiElement->GetComponents<UI::UIRenderableComponent>(renderableComVector.data(), renderableComNum);

            CoreMath::Vector2 rectVertices[4];
            uiElement->mTransform.GetQuadWorldPoints(rectVertices);
            Render::ScissorRect elementSicssorRect;
            elementSicssorRect.Left = rectVertices[0].X + viewport.Width / 2 + viewport.TopLeftX;

            elementSicssorRect.Right = rectVertices[1].X + viewport.Width / 2 + viewport.TopLeftX;

            elementSicssorRect.Top = -1 * rectVertices[0].Y + viewport.Height / 2 + viewport.TopLeftY;

            elementSicssorRect.Bottom = -1 * rectVertices[2].Y + viewport.Height / 2 + viewport.TopLeftY;

            for (auto uiCom : renderableComVector)
            {

                UI::UIRenderableComponent *renderableCom = (UI::UIRenderableComponent *)uiCom;
                if (renderableCom->GetActiveState() == false)
                    continue;

                UIElementContext uiElementContex;
                //  uiElementContex.mUIElement = uiElement;
                uiElementContex.mUIRenderableComponent = renderableCom;
                uiElementContex.mBatchKey.mRenderChannelID = renderChannelID;
                uiElementContex.mBatchKey.mMaterialID = renderableCom->GetUIMeshComponentRef().mUIMaterial->GetID();
                uiElementContex.mBatchKey.mScissorRect = elementSicssorRect;
                uiElementContextVector.push_back(uiElementContex);
            }
        }

        for (auto childElement : uiElement->GetChildVector())
        {
            uiElementStack.push(childElement);
        }
    }
}

void Quad::UIRenderItemBuilder::BuildAndSubmitRenderItem(Render::RenderChannelID renderChannelID,
                                                         const Render::Viewport &viewport, int clientWidth,
                                                         int clientHeight)
{

    std::vector<UI::UIVertex> batchedVertexVector;
    std::vector<uint32_t> batchedIndexVector;
    // 버텍스, 인덱스들이 한 버퍼에 다 모일거다

    // 그러니 같은 렌더아이템에 속한 정점들이 연속되어야한다
    std::vector<UIElementContext> &uiElementContextVector = mUIElementContextVectorPerRenderChannel[renderChannelID];

    std::sort(uiElementContextVector.begin(), uiElementContextVector.end(),
              [](const UIElementContext &a, const UIElementContext &b) { return a.mBatchKey < b.mBatchKey; });

    std::vector<Render::RenderItem> renderItemVector;
    renderItemVector.reserve(uiElementContextVector.size());

    Render::IMaterialManager *gpuMaterialManager = Render::IMaterialManager::GetInstance();
    Core::GpuSamplerSystem *gpuSamplerSystem = Core::GpuSamplerSystem::GetInstance();

    // 동일한 머터리얼내에서 인스턴스들의 정점오프셋
    // index값에 추가적으로 더해지는값
    uint32_t vertexBaseOffset = 0;
    for (int i = 0; i < uiElementContextVector.size(); ++i)
    {

        Render::RenderItem *renderItem = nullptr;
        if (i == 0 || uiElementContextVector[i].mBatchKey != uiElementContextVector[i - 1].mBatchKey)
        {
            Render::RenderItem newRenderItem;

            newRenderItem.mScissor = uiElementContextVector[i].mBatchKey.mScissorRect;

            /* CoreAsset::Material *assetMaterial =
                 uiElementContextVector[i].mUIElement->mMeshComponent.mTemplateMaterial.Get();*/
            CoreAsset::Material *assetMaterial =
                uiElementContextVector[i].mUIRenderableComponent->GetUIMeshComponentRef().mUIMaterial;

            newRenderItem.mMaterialID = assetMaterial->GetGpuMaterialID();
            newRenderItem.mInstance.mInstanceCount = 1;
            /*     newRenderItem.mScissor.Left = viewport.TopLeftX;
                 newRenderItem.mScissor.Top = viewport.TopLeftY;
                 newRenderItem.mScissor.Right = viewport.TopLeftX + viewport.Width;
                 newRenderItem.mScissor.Bottom = viewport.TopLeftY + viewport.Height;*/

            // 바인딩하는 세이더 리소스정보
            const Render::ShaderResourceInfoSet &shaderResourceInfoSet =
                gpuMaterialManager->GetMaterialShaderResourceInfo(newRenderItem.mMaterialID);

            // tex resource
            for (int i = 0; i < shaderResourceInfoSet.mTextureShaderResourceInfoVector.size(); ++i)
            {
                // GPUMateiral에서바인딩할 리소스 정보를 가져와서 그거에맞춰서 설정해야한다.
                // 일단 리소스는 텍스처 하나만 바인딩한다고하자
                // 사실상 ui는 거의 고정이다
                Render::BindingGpuResource texShaderResoure;
                texShaderResoure.mName = shaderResourceInfoSet.mTextureShaderResourceInfoVector[i].mName;
                texShaderResoure.mType = shaderResourceInfoSet.mTextureShaderResourceInfoVector[i].mType;
                /*        texShaderResoure.gpuResource = uiElementContextVector[i]
                                                           .mUIElement->mMeshComponent.mTemplateMaterial.Get()
                                                           ->GetTexResourceContextList()[i]
                                                           .mTexture.Get()
                                                           ->GetGpuResource();*/

                texShaderResoure.gpuResource = uiElementContextVector[i]
                                                   .mUIRenderableComponent->GetUIMeshComponentRef()
                                                   .mUIMaterial->GetTexResourceContextList()[i]
                                                   .mTexture.Get()
                                                   ->GetGpuResource();

                newRenderItem.mBindingGpuResourceVector.push_back(texShaderResoure);
            }
            // sampler

            const std::vector<uint32_t> materialSamplerContextList = assetMaterial->GetSamplerResourceContextList();
            for (int i = 0; i < shaderResourceInfoSet.mSamplerShaderResourceInfoVector.size(); ++i)
            {

                Render::BindingGpuResource samplerShaderResoure;
                samplerShaderResoure.mName = shaderResourceInfoSet.mSamplerShaderResourceInfoVector[i].mName;
                samplerShaderResoure.mType = shaderResourceInfoSet.mSamplerShaderResourceInfoVector[i].mType;
                GRM::GRMPtr samplerResource = gpuSamplerSystem->GetGpuSampler(materialSamplerContextList[i]);
                samplerShaderResoure.gpuResource = samplerResource.getResource();
                newRenderItem.mBindingGpuResourceVector.push_back(samplerShaderResoure);
            }

            // meshItem
            newRenderItem.mMeshItem.mVertexOffset = mCurrVertexBufferOffset + batchedVertexVector.size();
            newRenderItem.mMeshItem.mVertexNum = 0;
            newRenderItem.mMeshItem.mVertexBuffer = mVertexBufferGpu.getResource();
            newRenderItem.mMeshItem.mIndexBuffer = mIndexBufferGpu.getResource();
            newRenderItem.mMeshItem.mIndexNum = 0;
            newRenderItem.mMeshItem.mIndexOffset = mCurrIndexBufferOffset + batchedIndexVector.size();

            renderItemVector.push_back(newRenderItem);

            // 새로운머터리얼에대한 렌더아이템임으로  0으로 초기화
            vertexBaseOffset = 0;
        }

        // vertexnum 정보를 얻고
        size_t vertexNum = uiElementContextVector[i].mUIRenderableComponent->GetVertexNum();

        // vertex들을 얻는다..
        std::vector<UI::UIVertex> vertices(vertexNum);
        uiElementContextVector[i].mUIRenderableComponent->GetVertices(vertices.data());

        // indexNum정보를 얻고
        size_t indexNum = uiElementContextVector[i].mUIRenderableComponent->GetIndexNum();

        // index들을 얻는다...
        std::vector<uint32_t> indices(indexNum);
        uiElementContextVector[i].mUIRenderableComponent->GetIndices(indices.data());
        // index는 vertexbaseoffset을 추가로 더해준다.

        renderItem = &renderItemVector.back();
        renderItem->mMeshItem.mVertexNum += vertexNum;
        renderItem->mMeshItem.mIndexNum += indexNum;

        // vertex들의 pos를 스크린공간으로 보낸다.
        for (int j = 0; j < vertexNum; ++j)
        {
            float ndcX = (vertices[j].mPos.X / viewport.Width * 2);
            float ndcY = (vertices[j].mPos.Y / viewport.Height * 2);

            vertices[j].mPos = CoreMath::Vector2(ndcX, ndcY);
        }

        // 인덱스값들에 vertexBaseOffset을 추가로 더해준다.
        for (size_t i = 0; i < indexNum; ++i)
        {
            indices[i] += vertexBaseOffset;
        }
        batchedIndexVector.insert(batchedIndexVector.end(), indices.begin(), indices.end());
        /*     batchedIndexVector.push_back(vertexBaseOffset + 0);
             batchedIndexVector.push_back(vertexBaseOffset + 1);
             batchedIndexVector.push_back(vertexBaseOffset + 3);
             batchedIndexVector.push_back(vertexBaseOffset + 1);
             batchedIndexVector.push_back(vertexBaseOffset + 2);
             batchedIndexVector.push_back(vertexBaseOffset + 3);*/

        // 더 효율적으로 추가할수있는 메서드가있다.
        batchedVertexVector.insert(batchedVertexVector.end(), vertices.begin(), vertices.end());

        vertexBaseOffset += vertexNum;
    }

    // gpu 리소스로 데이터 업로드
    mGpuResourceManager->UploadBufferData(mVertexBufferGpu, batchedVertexVector.data(), sizeof(UI::UIVertex),
                                          batchedVertexVector.size(), mCurrVertexBufferOffset * sizeof(UI::UIVertex));

    mGpuResourceManager->UploadBufferData(mIndexBufferGpu, batchedIndexVector.data(), sizeof(uint32_t),
                                          batchedIndexVector.size(), mCurrIndexBufferOffset * sizeof(uint32_t));
    // 렌더아이템제출
    mRenderSystem->SubmitRenderItems(renderChannelID, renderItemVector);

    uiElementContextVector.clear();
    // 다음 호출을 위한 준비
    mCurrVertexBufferOffset += batchedVertexVector.size();
    mCurrIndexBufferOffset += batchedIndexVector.size();
}

void Quad::UIRenderItemBuilder::EndFrame()
{

    mUIElementContextVectorPerRenderChannel.clear();
    mCurrVertexBufferOffset = 0;
    mCurrIndexBufferOffset = 0;
}