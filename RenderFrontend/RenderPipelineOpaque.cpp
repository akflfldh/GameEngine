#include "RenderPipelineOpaque.h"
#include <CoreAsset/Material.h>
#include <CoreAsset/StaticMesh.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/GpuSamplerSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <Logger/Logger.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderSystem/IMaterialManager.h>

Render::RenderPipelineOpaque::RenderPipelineOpaque() {}

Render::RenderPipelineOpaque::~RenderPipelineOpaque() {}

// void Render::RenderPipelineOpaque::Execute(const std::vector<Core::RenderProxy *> &renderProxyList)
//{
//
//     //// main pass를 위한 renderItem 리스트 생성
//     // std::vector<RenderItem> mainPassRenderItemList = GetMainPassRenderItem(renderProxyList);
//
//     //// 다른 pass를 위한 renderItem 리스트 생성
//     // std::vector<RenderItem> shadowPassRenderItemList;
// }

void Render::RenderPipelineOpaque::SetGlobalData(const Core::GlobalFrameData &globalFrameData)
{

    //  mMainPass.SetGlobalData(globalFrameData);
}

void Render::RenderPipelineOpaque::RegisterAllPass(RenderPassGraph *renderPassGraph)
{

    renderPassGraph->RegisterRenderPassCallback(
        "Opaque_MainPass", [pPass = &this->mMainPass](RenderPassGraphBuilder &builder) {
               
        }, [pPass = &this->mMainPass](const RenderPassExecuteContext & executeContext) {
            
             pPass->Execute(executeContext);
            });
}

std::vector<Render::RenderItem> Render::RenderPipelineOpaque::GetMainPassRenderItem(
    const std::vector<Core::RenderProxy *> &renderProxyList)
{

    // 지금은 구조적버퍼를 사용하지않은 머터리얼들이기때문에
    // 개별적으로 그냥다 renderItem을 만든다.

    std::vector<Render::RenderItem> renderItemList;
    for (auto renderProxy : renderProxyList)
    {

        if (Core::StaticMeshRenderProxy *staticMeshRenderProxy =
                dynamic_cast<Core::StaticMeshRenderProxy *>(renderProxy))
        {

            BuildStaticRenderItem(renderItemList, staticMeshRenderProxy);
        }
    }

    return renderItemList;
}

void Render::RenderPipelineOpaque::BuildStaticRenderItem(std::vector<Render::RenderItem> &oRenderItemList,
                                                         Core::StaticMeshRenderProxy *staticMeshRenderProxy)
{

    //    const std::vector<CoreAsset::SubMesh> &subMeshList = staticMeshRenderProxy->mStaticMesh->GetSubMeshVector();
    //
    //    AssetResolver *assetResolver = AssetResolver::GetInstance();
    //    GRM::GpuSamplerSystem *gpuSamplerSystem = GRM::GpuSamplerSystem::GetInstance();
    //    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();
    //    GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();
    //
    //    const Render::MeshGpuResourceContext &meshGpuResourceContext =
    //        assetResolver->GetMeshGpuResourceContext(staticMeshRenderProxy->mStaticMesh->GetID());
    //
    //    for (int i = 0; i < staticMeshRenderProxy->mSubMeshMaterialList.size(); ++i)
    //    {
    //        RenderItem renderItem;
    //        CoreAsset::Material *material = staticMeshRenderProxy->mSubMeshMaterialList[i];
    //
    // #pragma region Mesh
    //
    //        const CoreAsset::SubMesh &subMesh = subMeshList[i];
    //        renderItem.mMaterialID = staticMeshRenderProxy->mSubMeshMaterialList[i]->GetGpuMaterialID();
    //
    //        renderItem.mMeshItem.mIndexNum = subMesh.mIndexNum;
    //        renderItem.mMeshItem.mIndexOffset = subMesh.mIndexOffset;
    //        renderItem.mMeshItem.mVertexOffset = subMesh.mVertexOffset;
    //        renderItem.mMeshItem.mIndexBuffer = meshGpuResourceContext.mIndexBuffer.getResource();
    //        renderItem.mMeshItem.mVertexBuffer = meshGpuResourceContext.mVertexBuffer.getResource();
    //
    // #pragma endregion
    //
    //        // 셰이더 리소스 정보
    //        const Render::ShaderResourceInfoSet &shaderResourceInfoSet =
    //            IMaterialManager::GetInstance()->GetMaterialShaderResourceInfo(renderItem.mMaterialID);
    //
    //// tex
    // #pragma region Tex
    //         const auto &texList = material->GetTexResourceContextList();
    //         for (size_t i = 0; i < shaderResourceInfoSet.mObjectTextureShaderResourceInfoVector.size(); ++i)
    //         {
    //             Render::BindingGpuResource bind;
    //             bind.mName = shaderResourceInfoSet.mObjectTextureShaderResourceInfoVector[i].mName;
    //             bind.mType = Render::EShaderResourceType::eTexture;
    //
    //             if (i < texList.size())
    //             {
    //                 bind.gpuResource = assetResolver->GetGpuResource(texList[i].mTexture.Get()).getResource();
    //                 if (bind.gpuResource == nullptr)
    //                 {
    //                     assetResolver->RequestResolveAsset(texList[i].mTexture.Get());
    //                     bind.gpuResource = assetResolver->GetGpuResource(texList[i].mTexture.Get()).getResource();
    //                 }
    //             }
    //             else
    //             {
    //                 // 매핑이 없으면 로그 남기고 nullptr로 둠
    //                 LOG_MESSAGE_INFO("ObjectRenderItemBuilder",
    //                                  ("Texture binding missing for shader resource: " + bind.mName).c_str());
    //                 bind.gpuResource = nullptr;
    //             }
    //
    //             renderItem.mBindingGpuResourceVector.push_back(std::move(bind));
    //         }
    // #pragma endregion
    //
    //// sampler
    // #pragma region Sampler
    //
    //         const auto &samplerContextList = material->GetSamplerResourceContextList();
    //
    //         for (size_t i = 0; i < shaderResourceInfoSet.mObjectSamplerShaderResourceInfoVector.size(); ++i)
    //         {
    //             Render::BindingGpuResource bind;
    //             bind.mName = shaderResourceInfoSet.mObjectSamplerShaderResourceInfoVector[i].mName;
    //             bind.mType = Render::EShaderResourceType::eSampler;
    //
    //             if (i < samplerContextList.size())
    //             {
    //                 GRM::GRMPtr samplerResource = gpuSamplerSystem->GetGpuSampler(samplerContextList[i]);
    //                 bind.gpuResource = samplerResource.getResource();
    //             }
    //
    //             renderItem.mBindingGpuResourceVector.push_back(std::move(bind));
    //         }
    //
    // #pragma endregion
    //
    //// buffer
    // #pragma region Buffer
    //
    //         //  현재 Object용 상수버퍼만 처리하고있다.
    //         for (size_t i = 0; i < shaderResourceInfoSet.mObjectBufferShaderResourceInfoVector.size(); ++i)
    //         {
    //             const Render::BufferShaderResourceInfo &bufInfo =
    //                 shaderResourceInfoSet.mObjectBufferShaderResourceInfoVector[i];
    //             const GRM::GpuBufferContext *gpuBufferContext =
    //                 gpuBufferContextSystem->GetGpuBufferContext(bufInfo.mBufferID);
    //
    //             const GRM::BufferDesc &bufferDesc = gpuBufferContext->mBufferDesc;
    //             uint32_t elementSize = static_cast<uint32_t>(bufferDesc.mElementDataSize);
    //             uint32_t cbSize = bufInfo.mSize;
    //
    //             std::vector<uint8_t> tmp(cbSize);
    //             if (bufInfo.mCreateBufferData)
    //             {
    //                 bufInfo.mCreateBufferData(staticMeshRenderProxy, nullptr, tmp.data());
    //             }
    //
    //             uint32_t elementIndex = gpuBufferContext->mAllocateRange.UseRange(1);
    //             Render::BindingGpuResource bindingGpuResource;
    //
    //             bindingGpuResource.mOffset = elementIndex;
    //             size_t byteOffset = static_cast<size_t>(elementIndex) * elementSize;
    //
    //             gpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, tmp.data(), elementSize, 1,
    //             byteOffset);
    //
    //             renderItem.mBindingGpuResourceVector.push_back(std::move(bindingGpuResource));
    //         }
    //
    // #pragma endregion
    //
    //         oRenderItemList.push_back(std::move(renderItem));
    //     }

    // 리소스 업로드
}
