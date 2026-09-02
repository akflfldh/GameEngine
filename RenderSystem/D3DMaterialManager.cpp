#include "pch.h"

#include "D3DGpuResourceManager/D3DGpuBuffer.h"
#include "D3DGpuResourceManager/D3DGpuTexture.h"
#include "RenderSystem/D3DMaterialManager.h"
#include "RenderSystem/D3DShaderReflectSystem.h"
#include "RenderSystem/IMaterialManager.h"
#include "RenderSystem/MaterialType.h"
#include <Logger.h>
#include <assert.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <fstream>
#include <vector>

D3DRender::D3DMaterialManager::D3DMaterialManager(Microsoft::WRL::ComPtr<ID3D12Device> device,
                                                  D3DGRM::D3DGpuResourceManager *gpuResourceManager)
    : mDevice(device), mGpuResourceManager(gpuResourceManager), mNextMaterialID(1)
{
    InitD3DBlendTable();
    InitD3DBlendOpTable();
    mMasterRootSignature = CreateRootMasterSignature();
}

D3DRender::D3DMaterialManager::~D3DMaterialManager() {}

// Render::MaterialID D3DRender::D3DMaterialManager::CreateMaterial(
//     const Render::CreationMaterialInfo &creationMaterialInfo)
//{
//
//     // 맨처음 중복확인
//     // creationMaterialInfo.mName; 할수도있고 필요없을수도
//
//     D3DMaterialItem materialItem;
//     materialItem.mName = creationMaterialInfo.mName;
//
//     // 세이더에서 사용하는 리소스들에 대한정보를 정리한다.
//
//     // 일단 이렇게 복사하는것만으로도 shaderResource들의 정보가 유지된다.
//     materialItem.mShaderResourceInfoSet = creationMaterialInfo.mShaderResourceInfoSet;
//
//     bool ret = BuildMainPass(creationMaterialInfo, materialItem.mMainPass, materialItem.mShaderResourceInfoSet);
//     if (ret == false)
//     {
//         return MaterialIDNone;
//     }
//
//     // 향후 확장
//     // engine default shader.
//     // post shdader 를 처리한다.(리플렉션 시스템도 가동한다)
//
//     Render::MaterialID id = GetNextMaterialID();
//     mMaterialItemTable[id] = std::make_unique<D3DRender::D3DMaterialItem>(std::move(materialItem));
//     mMaterialItemTable[id]->mID = id;
//     return id;
// }

// D3DRender::D3DMaterialItem *D3DRender::D3DMaterialManager::GetMaterialItem(Render::MaterialID materialID) const
//{
//
//     std::unordered_map<Render::MaterialID, std::unique_ptr<D3DMaterialItem>>::const_iterator it =
//         mMaterialItemTable.find(materialID);
//
//     if (it == mMaterialItemTable.cend())
//         return nullptr;
//
//     return it->second.get();
// }
//
// D3DRender::D3DMaterialItem *D3DRender::D3DMaterialManager::GetMaterialItem(const char *materialName) const
//{
//
//     std::unordered_map<Render::MaterialID, std::unique_ptr<D3DMaterialItem>>::const_iterator it = std::find_if(
//         mMaterialItemTable.begin(), mMaterialItemTable.end(),
//         [materialName](const std::pair<const Render::MaterialID, std::unique_ptr<D3DMaterialItem>> &element)
//         {
//             if (element.second->mName == materialName)
//                 return true;
//
//             return false;
//         });
//
//     if (it == mMaterialItemTable.cend())
//         return nullptr;
//
//     return it->second.get();
// }

const Render::ShaderResourceInfoSet &D3DRender::D3DMaterialManager::GetMaterialShaderResourceInfo(
    Render::MaterialID matID)
{

    std::unordered_map<Render::MaterialID, std::unique_ptr<D3DMaterialItem>>::iterator it =
        mMaterialItemTable.find(matID);

    if (it == mMaterialItemTable.end())
    {
        LOG_MESSAGE_CRITICAL("MaterialManager",
                             "Fun: GetMaterialShaderResourceInfo , 해당 Gpu머터리얼은 존재하지않습니다.");

        assert(0);
    }

    return it->second->mShaderResourceInfoSet;
}

const Render::ShaderResourceInfoSet &D3DRender::D3DMaterialManager::GetMaterialShaderResourceInfo(
    const char *materialName) const
{

    for (const auto &it : mMaterialItemTable)
    {

        if (it.second->mName == materialName)
        {
            return it.second.get()->mShaderResourceInfoSet;
        }
    }

    LOG_MESSAGE_CRITICAL("MaterialManager",
                         "Fun: GetMaterialShaderResourceInfo , 해당 Gpu머터리얼은 존재하지않습니다.");

    assert(0);
}

Render::ShaderResourceInfoSet &D3DRender::D3DMaterialManager::GetMaterialShaderResourceInfo(const char *materialName)
{

    return const_cast<Render::ShaderResourceInfoSet &>(
        const_cast<const D3DRender::D3DMaterialManager *>(this)->GetMaterialShaderResourceInfo(materialName));
}

Render::MaterialID D3DRender::D3DMaterialManager::CreateMaterial(const Render::MaterialGenerationInfo &info)
{
    // Material Root Sinature 사용한다.
    std::unordered_map<Render::EShaderStage, Microsoft::WRL::ComPtr<ID3DBlob>> compiledShaderTable;

    std::vector<uint8_t> shaderCode;
    CreateHLSL(info.mHLSLGenerationInfo, shaderCode);

    CompileHLSL(info, compiledShaderTable);

    ID3D12PipelineState *pso = CreatePSO(info, compiledShaderTable);

    if (pso)
    {
        auto materialItem = std::make_unique<D3DRender::MaterialItem>();
        Render::MaterialID matID = materialItem->mID = GetNextMaterialID();
        materialItem->mPso = pso;
        mMaterialItemsTable[matID] = std::move(materialItem);
        return matID;
    }

    return MaterialIDNone;
}

Render::MaterialID D3DRender::D3DMaterialManager::CreateMaterialDirectly(const Render::MaterialGenerationInfo &info/*,
                                                                         uint8_t *pShader, size_t shaderSize*/)
{
    // Material Root Sinature 사용한다.
    std::unordered_map<Render::EShaderStage, Microsoft::WRL::ComPtr<ID3DBlob>> compiledShaderTable;

    // 직접세이더코드가 들어오기때문에 HLSL을 생성할필요는없다 따라서 CreateHLSL은 수행하지않는다.

    CompileHLSL(info, compiledShaderTable);

    ID3D12PipelineState *pso = CreatePSO(info, compiledShaderTable);

    if (pso)
    {
        auto materialItem = std::make_unique<D3DRender::MaterialItem>();
        Render::MaterialID matID = materialItem->mID = GetNextMaterialID();
        materialItem->mPso = pso;
        mMaterialItemsTable[matID] = std::move(materialItem);
        return matID;
    }

    return MaterialIDNone;
}

D3DRender::MaterialItem *D3DRender::D3DMaterialManager::GetMaterialItem(Render::MaterialID materialID) const
{

    auto it = mMaterialItemsTable.find(materialID);
    if (it != mMaterialItemsTable.end())
    {
        return it->second.get();
    }

    return nullptr;
}

ID3D12RootSignature *D3DRender::D3DMaterialManager::GetMasterRootSignature() const
{
    return mMasterRootSignature.Get();
}

bool D3DRender::D3DMaterialManager::BuildMainPass(const Render::CreationMaterialInfo &creationMaterialInfo,
                                                  D3DMainRenderPassInfo &oMaterialItem,
                                                  Render::ShaderResourceInfoSet oShaderResourceInfo)
{
    // main pass

    // main pass 기본설정
    oMaterialItem.mIsEnginePass = creationMaterialInfo.mMainRenderPass.mDefaultFlag;
    oMaterialItem.mOpaqueFlag = creationMaterialInfo.mMainRenderPass.mOpaque;
    oMaterialItem.mInputType = creationMaterialInfo.mMainRenderPass.mInputType;

    // default pass를사용하는게 아니라면 나머지설정도 유저의 설정을 가진다.

    std::unordered_map<std::string, ShaderResourceReflectInfo> shaderResourceReflectInfoTable;
    if (oMaterialItem.mIsEnginePass == false)
    {

        auto &mainRenderPass = creationMaterialInfo.mMainRenderPass;

        // stage를 돌면서 shaderResource들의 reflectInfo를 얻는다.
        std::unordered_map<Render::EShaderStage, ID3DBlob *> compiledShaderTable;
        for (int shaderStageIndex = 0; shaderStageIndex < mainRenderPass.mShaderStage.size(); ++shaderStageIndex)
        {

            // 각 셰이더 스테이지 컴파일
            ID3DBlob *compiledBlob = nullptr;
            bool ret = CompileShader(mainRenderPass.mShaderStage[shaderStageIndex],
                                     creationMaterialInfo.mMainRenderPass.mShaderCode, compiledBlob);

            if (ret == false)
            {
                // 실패
                LOG_MESSAGE_ERROR("MaterialManager", "세이더 컴파일 실패");
                return false;
            }
            compiledShaderTable[mainRenderPass.mShaderStage[shaderStageIndex].mStage] = compiledBlob;

            // reflect를 수행하여 리소스 메타데이터 확보
            std::vector<ShaderResourceReflectInfo> shaderResourceReflectInfoVector;
            ret = D3DShaderReflectSystem::Reflect(compiledBlob->GetBufferPointer(), compiledBlob->GetBufferSize(),
                                                  shaderResourceReflectInfoVector);

            if (ret == false)
            {
                // 실패
                LOG_MESSAGE_ERROR("MaterialManager", "세이더 Reflect 실패");
                return false;
            }

            // shader파일의 내용과 리플렉션정보과 일치하는지 검증
            // 리소스들의 메타정보저장
            for (const auto &shaderResourceReflectInfo : shaderResourceReflectInfoVector)
            {
                shaderResourceReflectInfoTable.try_emplace(shaderResourceReflectInfo.mName, shaderResourceReflectInfo);
            }
        }

        bool ret = VertifyShaderResource(shaderResourceReflectInfoTable, creationMaterialInfo);
        if (ret == false)
        {
            // log
            return false;
        }

        oMaterialItem.mRootSignature = BuildRootSignature(shaderResourceReflectInfoTable, oMaterialItem);
        if (oMaterialItem.mRootSignature == nullptr)
        {
            return false;
        }

        oMaterialItem.mPSO =
            BuildPipelineState(oMaterialItem.mRootSignature.Get(), mainRenderPass, compiledShaderTable);
        if (oMaterialItem.mPSO == nullptr)
        {
            return false;
        }

        // oShaderResourceInfo내의ㅣ Resource들에 reflect로 얻은정보를통해 업데이트한다.

        for (const auto &reflectInfoElement : shaderResourceReflectInfoTable)
        {
            const std::string &resourceName = reflectInfoElement.first;
            const ShaderResourceReflectInfo &reflectInfo = reflectInfoElement.second;

            if (reflectInfo.mResourceType == Render::EShaderResourceType::eConstantBuffer)
            {
                std::vector<Render::BufferShaderResourceInfo>::iterator it =
                    std::find_if(oShaderResourceInfo.mObjectBufferShaderResourceInfoVector.begin(),
                                 oShaderResourceInfo.mObjectBufferShaderResourceInfoVector.end(),
                                 [&resourceName](const Render::BufferShaderResourceInfo &bufferInfo)
                                 {
                                     if (resourceName == bufferInfo.mName)
                                         return true;
                                     return false;
                                 });
                if (it != oShaderResourceInfo.mObjectBufferShaderResourceInfoVector.end())
                {
                    it->mSize = reflectInfo.mSize;
                }
                else
                {
                    std::vector<Render::BufferShaderResourceInfo>::iterator it =
                        std::find_if(oShaderResourceInfo.mPassBufferShaderResourceInfoVector.begin(),
                                     oShaderResourceInfo.mPassBufferShaderResourceInfoVector.end(),
                                     [&resourceName](const Render::BufferShaderResourceInfo &bufferInfo)
                                     {
                                         if (resourceName == bufferInfo.mName)
                                             return true;
                                         return false;
                                     });

                    it->mSize = reflectInfo.mSize;
                }
            }
        }
    }

    return true;
}

bool D3DRender::D3DMaterialManager::CompileShader(const Render::ShaderSourceInfo &shaderSourceInfo,
                                                  const std::vector<uint8_t> &shaderCode, ID3DBlob *&oCompiledBlob)
{

    ID3DBlob *errorBlob = nullptr;
    HRESULT ret = D3DCompile(shaderCode.data(), shaderCode.size(), nullptr, nullptr, nullptr,
                             shaderSourceInfo.mEntryPoint.c_str(), shaderSourceInfo.mTarget.c_str(), D3DCOMPILE_DEBUG,
                             0, &oCompiledBlob, &errorBlob);

    if (FAILED(ret))
    {

        const char *error = (const char *)errorBlob->GetBufferPointer();
        LOG_MESSAGE_ERROR("ShaderCompile", error);
        OutputDebugStringA(error);
        return false;
    }

    return true;
}

bool D3DRender::D3DMaterialManager::VertifyShaderResource(
    const std::unordered_map<std::string, ShaderResourceReflectInfo> &shaderResourceReflectInfoTable,
    const Render::CreationMaterialInfo &creationMaterialInfo)
{

    for (auto &shaderResourceReflectInfoElement : shaderResourceReflectInfoTable)
    {
        const std::string &resourceName = shaderResourceReflectInfoElement.first;

        const ShaderResourceReflectInfo &reflectInfo = shaderResourceReflectInfoElement.second;

        const Render::ShaderResourceInfoSet &resourceInfoSet = creationMaterialInfo.mShaderResourceInfoSet;

        switch (reflectInfo.mResourceType)
        {
        case Render::EShaderResourceType::eConstantBuffer:
        case Render::EShaderResourceType::eStructuredBuffer:
        {
            bool bFindResource = false;
            std::vector<Render::BufferShaderResourceInfo>::const_iterator it =
                std::find_if(resourceInfoSet.mObjectBufferShaderResourceInfoVector.cbegin(),
                             resourceInfoSet.mObjectBufferShaderResourceInfoVector.cend(),
                             [&resourceName](const Render::BufferShaderResourceInfo &bufferInfo)
                             {
                                 if (bufferInfo.mName == resourceName)
                                 {
                                     return true;
                                 }

                                 return false;
                             });

            if (it == resourceInfoSet.mObjectBufferShaderResourceInfoVector.cend())
            {
                it = std::find_if(resourceInfoSet.mPassBufferShaderResourceInfoVector.cbegin(),
                                  resourceInfoSet.mPassBufferShaderResourceInfoVector.cend(),
                                  [&resourceName](const Render::BufferShaderResourceInfo &bufferInfo)
                                  {
                                      if (bufferInfo.mName == resourceName)
                                      {
                                          return true;
                                      }

                                      return false;
                                  });

                if (it != resourceInfoSet.mPassBufferShaderResourceInfoVector.cend())
                {
                    bFindResource = true;
                }
            }
            else
            {
                bFindResource = true;
            }

            // 일치하는 이름의 리소스가 명시되어있지않다면 실패
            if (bFindResource == false)
            {
                // log
                return false;
            }

            // 차원이 다르다면 실패
            if (reflectInfo.mResourceDimension != it->mDimension)
            {
                return false;
            }

            // 타입이 다르다면 실패
            if (reflectInfo.mResourceType != it->mType)
            {
                return false;
            }
        }
        break;
        case Render::EShaderResourceType::eTexture:
        {
            bool bFindResource = false;
            std::vector<Render::TextureShaderResourceInfo>::const_iterator it =
                std::find_if(resourceInfoSet.mObjectTextureShaderResourceInfoVector.cbegin(),
                             resourceInfoSet.mObjectTextureShaderResourceInfoVector.cend(),
                             [&resourceName](const Render::TextureShaderResourceInfo &bufferInfo)
                             {
                                 if (bufferInfo.mName == resourceName)
                                 {
                                     return true;
                                 }

                                 return false;
                             });

            if (it == resourceInfoSet.mObjectTextureShaderResourceInfoVector.cend())
            {
                it = std::find_if(resourceInfoSet.mPassTextureShaderResourceInfoVector.cbegin(),
                                  resourceInfoSet.mPassTextureShaderResourceInfoVector.cend(),
                                  [&resourceName](const Render::TextureShaderResourceInfo &bufferInfo)
                                  {
                                      if (bufferInfo.mName == resourceName)
                                      {
                                          return true;
                                      }

                                      return false;
                                  });

                if (it != resourceInfoSet.mPassTextureShaderResourceInfoVector.cend())
                {
                    bFindResource = true;
                }
            }
            else
            {
                bFindResource = true;
            }

            // 일치하는 이름의 리소스가 명시되어있지않다면 실패
            if (bFindResource == false)
            {
                // log
                return false;
            }

            // 차원이 다르다면 실패
            if (reflectInfo.mResourceDimension != it->mDimension)
            {
                return false;
            }

            // 타입이 다르다면 실패
            if (reflectInfo.mResourceType != it->mType)
            {
                return false;
            }
        }
        break;
        }
    }

    return true;
}

ID3D12RootSignature *D3DRender::D3DMaterialManager::BuildRootSignature(
    const std::unordered_map<std::string, ShaderResourceReflectInfo> &shaderResourceReflectInfoTable,
    D3DRenderPassInfo &oRenderPassInfo)
{
    // D3DMainRenderPassInfo 를 채운다.
    // materialItem.mMainPass.mShaderResourceRootParameterBindingInfo;
    // materialItem.mMainPass.mRootSignature;

    int rootParameterIndex = 0;
    std::vector<D3D12_ROOT_PARAMETER> rootParameterVector(shaderResourceReflectInfoTable.size());
    std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRangeVector(shaderResourceReflectInfoTable.size());
    for (const auto &shaderResourceReflectInfoElement : shaderResourceReflectInfoTable)
    {

        const std::string &shaderResourceName = shaderResourceReflectInfoElement.first;
        const ShaderResourceReflectInfo &resourceReflectInfo = shaderResourceReflectInfoElement.second;

        // 루트 시그니처를 생성한다.
        // 루트 파라미터를 생성한다.

        rootParameterVector[rootParameterIndex].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        // 일단은 이렇게고정
        rootParameterVector[rootParameterIndex].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameterVector[rootParameterIndex].DescriptorTable.NumDescriptorRanges = 1;
        rootParameterVector[rootParameterIndex].DescriptorTable.pDescriptorRanges =
            &descriptorRangeVector[rootParameterIndex];

        descriptorRangeVector[rootParameterIndex].BaseShaderRegister = resourceReflectInfo.mRegisterIndex;
        descriptorRangeVector[rootParameterIndex].RegisterSpace = resourceReflectInfo.mRegisterSpace;
        descriptorRangeVector[rootParameterIndex].NumDescriptors = 1;
        descriptorRangeVector[rootParameterIndex].RangeType =
            ConvertToDescriptorRangeType(resourceReflectInfo.mResourceType);
        descriptorRangeVector[rootParameterIndex].OffsetInDescriptorsFromTableStart = 0;

        // 루트 파라미터의 index를 넣는다.
        oRenderPassInfo.mShaderResourceRootParameterBindingInfo[shaderResourceName] = rootParameterIndex++;
    }

    // D3D12RootSignature 생성
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.NumParameters = rootParameterVector.size();
    rootSignatureDesc.pParameters = rootParameterVector.data();
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob *serializedRootSignatureBlob;
    ID3DBlob *errorBlob;
    HRESULT ret = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
                                              &serializedRootSignatureBlob, &errorBlob);

    if (FAILED(ret))
    {
        LOG_MESSAGE_ERROR("MaterialManager", (char *)errorBlob->GetBufferPointer());
        return nullptr;
    }

    ID3D12RootSignature *rootSignature = nullptr;
    ret = mDevice->CreateRootSignature(0, serializedRootSignatureBlob->GetBufferPointer(),
                                       serializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

    if (FAILED(ret))
    {

        LOG_MESSAGE_ERROR("MaterialManager", "루트시그니처 생성 실패");
        return nullptr;
    }

    return rootSignature;
}

ID3D12PipelineState *D3DRender::D3DMaterialManager::BuildPipelineState(
    ID3D12RootSignature *rootSignature, const Render::MainRenderPass &mainRenderPass,
    const std::unordered_map<Render::EShaderStage, ID3DBlob *> &compiledShaderTable)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = D3D12_GRAPHICS_PIPELINE_STATE_DESC{};

    pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    // Input layout
    // 스키닝이냐, 정적메시냐에 따라서 inputLayout이 달라진다.
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescVector;
    BuildIMainInputLayout(mainRenderPass, inputElementDescVector);
    D3D12_INPUT_LAYOUT_DESC inputLayout = {inputElementDescVector.data(), inputElementDescVector.size()};

    pipelineStateDesc.RasterizerState.CullMode = ConvertToD3DCullMode(mainRenderPass.mCullMode);
    pipelineStateDesc.RasterizerState.FillMode = ConvertToD3DFillMode(mainRenderPass.mFillMode);
    pipelineStateDesc.RasterizerState.FrontCounterClockwise = mainRenderPass.mCCW;
    pipelineStateDesc.SampleMask = 0xFFFFFFFF;
    pipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = ConvertToBlendEnableFlag(mainRenderPass.mBlendMode);
    pipelineStateDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    if (pipelineStateDesc.BlendState.RenderTarget[0].BlendEnable)
    {
        //   pipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = ConvertToD3DBlend(mainRenderPass.mBlendSrc);
        //  pipelineStateDesc.BlendState.RenderTarget[0].DestBlend = ConvertToD3DBlend(mainRenderPass.mBlendDest);
        //  pipelineStateDesc.BlendState.RenderTarget[0].BlendOp = ConvertToD3DBlendOp(mainRenderPass.mBlendOp);
    }

    pipelineStateDesc.DepthStencilState.DepthFunc = ConvertToD3DCmpFun(mainRenderPass.mDepthCompareMode);
    pipelineStateDesc.DepthStencilState.DepthEnable = ConvertToDepthWriteModeEnableFlag(mainRenderPass.mDepthWriteMode);

    pipelineStateDesc.DepthStencilState.StencilEnable =
        ConvertToStencilWriteModeEnableFlag(mainRenderPass.mStencilWriteMode);

    if (pipelineStateDesc.DepthStencilState.StencilEnable)
    {
        pipelineStateDesc.DepthStencilState.FrontFace.StencilFunc =
            ConvertToD3DCmpFun(mainRenderPass.mStencilFrontCompareMode);
        pipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp =
            ConvertToD3DStencilOP(mainRenderPass.mStencilFrontPassOp);
        pipelineStateDesc.DepthStencilState.FrontFace.StencilFailOp =
            ConvertToD3DStencilOP(mainRenderPass.mStencilFrontFailOp);
    }

    //    pipelineStateDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL;

    pipelineStateDesc.pRootSignature = rootSignature;
    pipelineStateDesc.InputLayout = inputLayout;
    pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 일단 기본설정
    pipelineStateDesc.SampleDesc.Count = 1;
    pipelineStateDesc.SampleDesc.Quality = 0;
    pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pipelineStateDesc.NumRenderTargets = 1;
    pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // 일단 기본설정

    // VS
    std::unordered_map<Render::EShaderStage, ID3DBlob *>::const_iterator vsIt =
        compiledShaderTable.find(Render::EShaderStage::eVertex);
    if (vsIt == compiledShaderTable.cend())
    {
        LOG_MESSAGE_ERROR("MaterialManager", "컴파일된 버텍스셰이더가 없습니다.");
        return nullptr;
    }
    pipelineStateDesc.VS.pShaderBytecode = vsIt->second->GetBufferPointer();
    pipelineStateDesc.VS.BytecodeLength = vsIt->second->GetBufferSize();

    // PS
    std::unordered_map<Render::EShaderStage, ID3DBlob *>::const_iterator psIt =
        compiledShaderTable.find(Render::EShaderStage::ePixel);
    if (psIt != compiledShaderTable.cend())
    {
        pipelineStateDesc.PS.pShaderBytecode = psIt->second->GetBufferPointer();
        pipelineStateDesc.PS.BytecodeLength = psIt->second->GetBufferSize();
    }

    // GS
    std::unordered_map<Render::EShaderStage, ID3DBlob *>::const_iterator gsIt =
        compiledShaderTable.find(Render::EShaderStage::eGeometry);
    if (gsIt != compiledShaderTable.cend())
    {
        pipelineStateDesc.GS.pShaderBytecode = gsIt->second->GetBufferPointer();
        pipelineStateDesc.GS.BytecodeLength = gsIt->second->GetBufferSize();
    }

    // DS
    std::unordered_map<Render::EShaderStage, ID3DBlob *>::const_iterator dsIt =
        compiledShaderTable.find(Render::EShaderStage::eDomain);
    if (dsIt != compiledShaderTable.cend())
    {
        pipelineStateDesc.DS.pShaderBytecode = dsIt->second->GetBufferPointer();
        pipelineStateDesc.DS.BytecodeLength = dsIt->second->GetBufferSize();
    }

    // HS
    std::unordered_map<Render::EShaderStage, ID3DBlob *>::const_iterator hsIt =
        compiledShaderTable.find(Render::EShaderStage::eHull);
    if (hsIt != compiledShaderTable.cend())
    {
        pipelineStateDesc.HS.pShaderBytecode = hsIt->second->GetBufferPointer();
        pipelineStateDesc.HS.BytecodeLength = hsIt->second->GetBufferSize();
    }

    ID3D12PipelineState *pipelineState = nullptr;
    HRESULT ret = mDevice->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState));
    if (FAILED(ret))
    {

        LOG_MESSAGE_ERROR("MaterialManager", "파이프라인 생성실패");
    }

    return pipelineState;
}

void D3DRender::D3DMaterialManager::BuildIMainInputLayout(
    const Render::MainRenderPass mainRenderPass, std::vector<D3D12_INPUT_ELEMENT_DESC> &oInputElementDescVector)
{

    // 현재 이거는 고정된것 특히 MainPass는
    if (mainRenderPass.mInputType == 0)
    {
        // ui

        oInputElementDescVector.push_back(
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back(
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back(
            {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back({"COMMON", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
        oInputElementDescVector.push_back({"COMMON", 1, DXGI_FORMAT_R32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
        oInputElementDescVector.push_back({"COMMON", 2, DXGI_FORMAT_R32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    }
    else if (mainRenderPass.mInputType == 3)
    {
        return;
    }
    else if (mainRenderPass.mInputType >= 1)
    {
        oInputElementDescVector.push_back({"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back({"TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back({"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back({"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
                                           D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                                           0});

        if (mainRenderPass.mInputType == 2)
        {
            // 추가로
            // bone index
            // bone weight
            // 2걔씩
            oInputElementDescVector.push_back({"BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
                                               D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                                               0});

            oInputElementDescVector.push_back({"BONEINDEX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
                                               D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                                               0});

            oInputElementDescVector.push_back({"BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
                                               D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                                               0});

            oInputElementDescVector.push_back({"BONEWEIGHT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
                                               D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                                               0});
        }
    }

    return;
}

void D3DRender::D3DMaterialManager::BuildIMainInputLayout(
    Render::EInputLayoutType inputLayoutType, std::vector<D3D12_INPUT_ELEMENT_DESC> &oInputElementDescVector)
{
    // 현재 이거는 고정된것 특히 MainPass는
    if (inputLayoutType == Render::EInputLayoutType::eUI)
    {
        // ui

        oInputElementDescVector.push_back(
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back(
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back(
            {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back({"COMMON", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
        oInputElementDescVector.push_back({"COMMON", 1, DXGI_FORMAT_R32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
        oInputElementDescVector.push_back({"COMMON", 2, DXGI_FORMAT_R32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    }
    else if (inputLayoutType == Render::EInputLayoutType::eBillboard)
    {
        // 빌보드용
        // 레이아웃이 없다.
        return;
    }
    else if (inputLayoutType == Render::EInputLayoutType::eLine)
    {

        oInputElementDescVector.push_back({"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
                                           D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                                           0});

        oInputElementDescVector.push_back({"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    }
    else if (inputLayoutType >= Render::EInputLayoutType::eStaticMesh)
    {
        oInputElementDescVector.push_back({"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back({"TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back({"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back({"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
                                           D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                                           0});

        if (inputLayoutType == Render::EInputLayoutType::eSkinningMesh)
        {
            // 추가로
            // bone index
            // bone weight
            // 2걔씩
            oInputElementDescVector.push_back({"BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
                                               D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                                               0});

            oInputElementDescVector.push_back({"BONEINDEX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
                                               D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                                               0});

            oInputElementDescVector.push_back({"BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
                                               D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                                               0});

            oInputElementDescVector.push_back({"BONEWEIGHT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
                                               D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                                               0});
        }
    }

    return;
}

D3D12_DESCRIPTOR_RANGE_TYPE D3DRender::D3DMaterialManager::ConvertToDescriptorRangeType(
    Render::EShaderResourceType shaderResourceType)
{

    switch (shaderResourceType)
    {

    case Render::EShaderResourceType::eConstantBuffer:

        return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

    case Render::EShaderResourceType::eTexture:
    case Render::EShaderResourceType::eStructuredBuffer:
        return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

    case Render::EShaderResourceType::eSampler:

        return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    }
}

D3D12_CULL_MODE D3DRender::D3DMaterialManager::ConvertToD3DCullMode(Render::ECullMode cullMode)
{
    switch (cullMode)
    {
    case Render::ECullMode::eBack:
        return D3D12_CULL_MODE_BACK;

    case Render::ECullMode::eFront:

        return D3D12_CULL_MODE_FRONT;

    case Render::ECullMode::eNone:

        return D3D12_CULL_MODE_NONE;
    }
}

D3D12_FILL_MODE D3DRender::D3DMaterialManager::ConvertToD3DFillMode(Render::EFillMode fillMode)
{

    switch (fillMode)
    {
    case Render::EFillMode::eSolidMode:

        return D3D12_FILL_MODE_SOLID;

    case Render::EFillMode::eWireFrameMode:

        return D3D12_FILL_MODE_WIREFRAME;
    }
}

bool D3DRender::D3DMaterialManager::ConvertToBlendEnableFlag(Render::EBlendMode blendMode)
{
    switch (blendMode)
    {
    case Render::EBlendMode::eOpaque:
        return false;
    case Render::EBlendMode::eAlphaBlend:

        return true;
    }
}

D3D12_BLEND D3DRender::D3DMaterialManager::ConvertToD3DBlend(Render::EBlend blend) const
{

    switch (blend)
    {
    case Render::EBlend::eBLEND_ZERO:
        return D3D12_BLEND_ZERO;
    case Render::EBlend::eBLEND_ONE:
        return D3D12_BLEND_ONE;
    case Render::EBlend::eBLEND_SRC_COLOR:
        return D3D12_BLEND_SRC_COLOR;
    case Render::EBlend::eBLEND_INV_SRC_COLOR:
        return D3D12_BLEND_INV_SRC_COLOR;
    case Render::EBlend::eBLEND_SRC_ALPHA:
        return D3D12_BLEND_SRC_ALPHA;
    case Render::EBlend::eBLEND_INV_SRC_ALPHA:
        return D3D12_BLEND_INV_SRC_ALPHA;
    case Render::EBlend::eBLEND_DEST_ALPHA:
        return D3D12_BLEND_DEST_ALPHA;
    case Render::EBlend::eBLEND_INV_DEST_ALPHA:
        return D3D12_BLEND_INV_DEST_ALPHA;
    case Render::EBlend::eBLEND_DEST_COLOR:
        return D3D12_BLEND_DEST_COLOR;
    case Render::EBlend::eBLEND_INV_DEST_COLOR:
        return D3D12_BLEND_INV_DEST_COLOR;
    case Render::EBlend::eBLEND_SRC_ALPHA_SAT:
        return D3D12_BLEND_SRC_ALPHA_SAT;
    case Render::EBlend::eBLEND_BLEND_FACTOR:
        return D3D12_BLEND_BLEND_FACTOR;
    case Render::EBlend::eBLEND_INV_BLEND_FACTOR:
        return D3D12_BLEND_INV_BLEND_FACTOR;
    case Render::EBlend::eBLEND_SRC1_COLOR:
        return D3D12_BLEND_SRC1_COLOR;
    case Render::EBlend::eBLEND_INV_SRC1_COLOR:
        return D3D12_BLEND_INV_SRC1_COLOR;
    case Render::EBlend::eBLEND_SRC1_ALPHA:
        return D3D12_BLEND_SRC1_ALPHA;
    case Render::EBlend::eBLEND_INV_SRC1_ALPHA:
        return D3D12_BLEND_INV_SRC1_ALPHA;
    case Render::EBlend::eALPHA_FACTOR:
        return D3D12_BLEND_ALPHA_FACTOR;
    case Render::EBlend::eINV_ALPHA_FACTOR:
        return D3D12_BLEND_INV_ALPHA_FACTOR;
    default:
        assert(false && "Unknown EBlend value");
        return D3D12_BLEND_ZERO;
    }
}

D3D12_BLEND_OP D3DRender::D3DMaterialManager::ConvertToD3DBlendOp(Render::EBlendOp blendOp) const
{

    switch (blendOp)
    {
    case Render::EBlendOp::eADD:
        return D3D12_BLEND_OP_ADD;
    case Render::EBlendOp::eSUBTRACT:
        return D3D12_BLEND_OP_SUBTRACT;
    case Render::EBlendOp::eREV_SUBTRACT:
        return D3D12_BLEND_OP_REV_SUBTRACT;
    case Render::EBlendOp::eMIN:
        return D3D12_BLEND_OP_MIN;
    case Render::EBlendOp::eMAX:
        return D3D12_BLEND_OP_MAX;
    default:
        assert(false && "Unknown EBlendOp value");
        return D3D12_BLEND_OP_ADD;
    }
}

D3D12_COMPARISON_FUNC D3DRender::D3DMaterialManager::ConvertToD3DCmpFun(Render::EDepthStencilCompareMode cmpMode)
{

    switch (cmpMode)
    {

    case Render::EDepthStencilCompareMode::eEqual:
        return D3D12_COMPARISON_FUNC_EQUAL;

    case Render::EDepthStencilCompareMode::eLess:

        return D3D12_COMPARISON_FUNC_LESS;

    case Render::EDepthStencilCompareMode::eLessEqual:

        return D3D12_COMPARISON_FUNC_LESS_EQUAL;

    case Render::EDepthStencilCompareMode::eNone:

        return D3D12_COMPARISON_FUNC_NONE;
    case Render::EDepthStencilCompareMode::eAlways:
        return D3D12_COMPARISON_FUNC_ALWAYS;
    case Render::EDepthStencilCompareMode::eNotEqual:
        return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    }

    return D3D12_COMPARISON_FUNC();
}

bool D3DRender::D3DMaterialManager::ConvertToDepthWriteModeEnableFlag(Render::EDepthWriteMode writeMode)
{

    switch (writeMode)
    {
    case Render::EDepthWriteMode::eDisabled:

        return false;

    case Render::EDepthWriteMode::eEnabled:

        return true;
    }
}

D3D12_DEPTH_WRITE_MASK D3DRender::D3DMaterialManager::ConvertToDepthWriteMask(bool writeMask)
{
    if (writeMask)
    {
        return D3D12_DEPTH_WRITE_MASK_ALL;
    }
    else
    {
        return D3D12_DEPTH_WRITE_MASK_ZERO;
    }
}

bool D3DRender::D3DMaterialManager::ConvertToStencilWriteModeEnableFlag(Render::EStencilWriteMode writeMode)
{
    switch (writeMode)
    {
    case Render::EStencilWriteMode::eDisabled:

        return false;

    case Render::EStencilWriteMode::eEnabled:

        return true;
    }
}

D3D12_STENCIL_OP D3DRender::D3DMaterialManager::ConvertToD3DStencilOP(Render::EStencilOP op)
{

    switch (op)
    {
    case Render::EStencilOP::eKeep:
        return D3D12_STENCIL_OP_KEEP;
    case Render::EStencilOP::eZero:
        return D3D12_STENCIL_OP_KEEP;
    case Render::EStencilOP::eReplace:
        return D3D12_STENCIL_OP_REPLACE;
    case Render::EStencilOP::eIncr_sat:
        return D3D12_STENCIL_OP_INCR_SAT;
    case Render::EStencilOP::eDecr_sat:
        return D3D12_STENCIL_OP_DECR_SAT;
    case Render::EStencilOP::eInvert:
        return D3D12_STENCIL_OP_INVERT;
    case Render::EStencilOP::eIncr:
        return D3D12_STENCIL_OP_INCR;
    case Render::EStencilOP::eDecr:
        return D3D12_STENCIL_OP_DECR;
    }

    return D3D12_STENCIL_OP();
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> D3DRender::D3DMaterialManager::CreateRootMasterSignature()
{

    // 1번 pass buffer

    // 2번 material buffer

    std::vector<D3D12_ROOT_PARAMETER> rootParameterVector(4 + MAX_TEXTURE_SLOT + 3);

    // 0번 pass buffer
    rootParameterVector[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameterVector[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameterVector[0].Descriptor.RegisterSpace = 0;
    rootParameterVector[0].Descriptor.ShaderRegister = 0;

    // 1번 object buffer
    rootParameterVector[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameterVector[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameterVector[1].Descriptor.RegisterSpace = 0;
    rootParameterVector[1].Descriptor.ShaderRegister = 1;

    // 2번 material buffer
    rootParameterVector[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameterVector[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameterVector[2].Descriptor.RegisterSpace = 0;
    rootParameterVector[2].Descriptor.ShaderRegister = 2;

    // 3번 light structured buffer
    rootParameterVector[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParameterVector[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameterVector[3].Descriptor.RegisterSpace = 0;
    rootParameterVector[3].Descriptor.ShaderRegister = 0;

    // 4 ~ 7번까지 tex
    D3D12_DESCRIPTOR_RANGE texDescriptorRange[MAX_TEXTURE_SLOT];
    int shaderRegisterOffset = 1;
    int parameterIndex = 4;
    for (int i = 0; i < MAX_TEXTURE_SLOT; ++i)
    {

        rootParameterVector[parameterIndex].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameterVector[parameterIndex].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameterVector[parameterIndex].DescriptorTable.NumDescriptorRanges = 1;
        rootParameterVector[parameterIndex].DescriptorTable.pDescriptorRanges = &texDescriptorRange[i];
        texDescriptorRange[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        texDescriptorRange[i].NumDescriptors = 1;
        texDescriptorRange[i].BaseShaderRegister = shaderRegisterOffset;
        texDescriptorRange[i].RegisterSpace = 0;
        texDescriptorRange[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        shaderRegisterOffset += 1;
        parameterIndex += 1;
    }

    int parameterOffset = 4 + MAX_TEXTURE_SLOT + 1;
    // 8번 ~ 10번까지 structuredBuffer
    for (int i = 0; i < 3; ++i)
    {
        rootParameterVector[parameterIndex].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        rootParameterVector[parameterIndex].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameterVector[parameterIndex].Descriptor.RegisterSpace = 0;
        rootParameterVector[parameterIndex].Descriptor.ShaderRegister = shaderRegisterOffset;

        parameterIndex++;
        shaderRegisterOffset++;
    }

    // TODO 함수로 분리하기 정적샘플러 생성
    //  정적 샘플러 사용
    D3D12_STATIC_SAMPLER_DESC staticSampler[2];
    staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler[0].MipLODBias = 0;
    staticSampler[0].MaxAnisotropy = 1;
    staticSampler[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
    staticSampler[0].MinLOD = 0;
    staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;
    staticSampler[0].ShaderRegister = 0;
    staticSampler[0].RegisterSpace = 0;
    staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    staticSampler[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    staticSampler[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler[1].MipLODBias = 0;
    staticSampler[1].MaxAnisotropy = 1;
    staticSampler[1].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
    staticSampler[1].MinLOD = 0;
    staticSampler[1].MaxLOD = D3D12_FLOAT32_MAX;
    staticSampler[1].ShaderRegister = 1;
    staticSampler[1].RegisterSpace = 0;
    staticSampler[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // D3D12RootSignature 생성
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.NumParameters = rootParameterVector.size();
    rootSignatureDesc.pParameters = rootParameterVector.data();
    rootSignatureDesc.pStaticSamplers = staticSampler;
    rootSignatureDesc.NumStaticSamplers = 2;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob *serializedRootSignatureBlob;
    ID3DBlob *errorBlob;
    HRESULT ret = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
                                              &serializedRootSignatureBlob, &errorBlob);

    if (FAILED(ret))
    {
        LOG_MESSAGE_ERROR("MaterialManager", (char *)errorBlob->GetBufferPointer());
        return nullptr;
    }

    ID3D12RootSignature *rootSignature = nullptr;
    ret = mDevice->CreateRootSignature(0, serializedRootSignatureBlob->GetBufferPointer(),
                                       serializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

    if (FAILED(ret))
    {
        const char *error = (const char *)errorBlob->GetBufferPointer();
        LOG_MESSAGE_ERROR("ShaderCompile", error);
        OutputDebugStringA(error);
        return nullptr;
    }

    return rootSignature;
}

bool D3DRender::D3DMaterialManager::CreateHLSL(const Render::MaterialHLSLGenerationInfo &hlslGenerationInfo,
                                               std::vector<uint8_t> &oShader)
{

    // HLSL 텍스트 코드만생성

    return true;
}

bool D3DRender::D3DMaterialManager::CompileHLSL(
    const Render::MaterialGenerationInfo &info /*, uint8_t *pShader, size_t size*/,
    std::unordered_map<Render::EShaderStage, Microsoft::WRL::ComPtr<ID3DBlob>> &oShaderTable)
{

    for (auto shaderStageInfo : info.mShaderInfoList)
    {

        Microsoft::WRL::ComPtr<ID3DBlob> blob = nullptr;
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
        ID3DBlob *errorBlob = nullptr;

        uint8_t *pShader = shaderStageInfo.mShadeCode;
        size_t shaderSize = shaderStageInfo.mShaderCodeSize;

        HRESULT ret = D3DCompile(pShader, shaderSize, nullptr, nullptr, nullptr, shaderStageInfo.mEntryPoint.c_str(),
                                 shaderStageInfo.mTarget.c_str(), compileFlags, 0, &blob, &errorBlob);

        if (FAILED(ret))
        {

            const char *error = (const char *)errorBlob->GetBufferPointer();
            LOG_MESSAGE_ERROR("ShaderCompile", error);
            OutputDebugStringA(error);
            return false;
        }

        oShaderTable[shaderStageInfo.mStage] = blob;
    }

    return true;
}

ID3D12PipelineState *D3DRender::D3DMaterialManager::CreatePSO(
    const Render::MaterialGenerationInfo &info,
    const std::unordered_map<Render::EShaderStage, Microsoft::WRL::ComPtr<ID3DBlob>> &shaderTable)
{

    // PSO 구축
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = D3D12_GRAPHICS_PIPELINE_STATE_DESC{};

    pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    // Input layout
    // 스키닝이냐, 정적메시냐에 따라서 inputLayout이 달라진다.
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescVector;
    BuildIMainInputLayout(info.mInputLayoutType, inputElementDescVector);
    D3D12_INPUT_LAYOUT_DESC inputLayout = {inputElementDescVector.data(), inputElementDescVector.size()};

    const Render::MaterialRenderSettingInfo &renderSettingInfo = info.mRenderSettingInfo;

    pipelineStateDesc.RasterizerState.CullMode = ConvertToD3DCullMode(renderSettingInfo.mCullMode);
    pipelineStateDesc.RasterizerState.FillMode = ConvertToD3DFillMode(renderSettingInfo.mFillMode);
    pipelineStateDesc.RasterizerState.FrontCounterClockwise = renderSettingInfo.mCCW;
    pipelineStateDesc.SampleMask = 0xFFFFFFFF;
    pipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = ConvertToBlendEnableFlag(renderSettingInfo.mBlendMode);
    pipelineStateDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    if (pipelineStateDesc.BlendState.RenderTarget[0].BlendEnable)
    {
        pipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = ConvertToD3DBlend(renderSettingInfo.mBlendSrc);
        pipelineStateDesc.BlendState.RenderTarget[0].DestBlend = ConvertToD3DBlend(renderSettingInfo.mBlendDest);
        pipelineStateDesc.BlendState.RenderTarget[0].BlendOp = ConvertToD3DBlendOp(renderSettingInfo.mBlendOp);
    }

    pipelineStateDesc.DepthStencilState.DepthFunc = ConvertToD3DCmpFun(renderSettingInfo.mDepthCompareMode);
    pipelineStateDesc.DepthStencilState.DepthEnable =
        ConvertToDepthWriteModeEnableFlag(renderSettingInfo.mDepthWriteMode);
    pipelineStateDesc.DepthStencilState.DepthWriteMask = ConvertToDepthWriteMask(renderSettingInfo.mDepthWriteMask);

    pipelineStateDesc.DepthStencilState.StencilEnable =
        ConvertToStencilWriteModeEnableFlag(renderSettingInfo.mStencilWriteMode);

    ;

    if (pipelineStateDesc.DepthStencilState.StencilEnable)
    {
        pipelineStateDesc.DepthStencilState.FrontFace.StencilFunc =
            ConvertToD3DCmpFun(renderSettingInfo.mStencilFrontCompareMode);
        pipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp =
            ConvertToD3DStencilOP(renderSettingInfo.mStencilFrontPassOp);
        pipelineStateDesc.DepthStencilState.FrontFace.StencilFailOp =
            ConvertToD3DStencilOP(renderSettingInfo.mStencilFrontFailOp);
    }

    //    pipelineStateDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL;

    pipelineStateDesc.pRootSignature = mMasterRootSignature.Get(); // master root signature
    pipelineStateDesc.InputLayout = inputLayout;
    pipelineStateDesc.PrimitiveTopologyType = GetPrimitiveTopologyType(info.mInputLayoutType);
    pipelineStateDesc.SampleDesc.Count = 1;
    pipelineStateDesc.SampleDesc.Quality = 0;
    pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pipelineStateDesc.NumRenderTargets = 1;
    pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // 일단 기본설정

    // VS
    std::unordered_map<Render::EShaderStage, Microsoft::WRL::ComPtr<ID3DBlob>>::const_iterator vsIt =
        shaderTable.find(Render::EShaderStage::eVertex);
    if (vsIt == shaderTable.cend())
    {
        LOG_MESSAGE_ERROR("MaterialManager", "컴파일된 버텍스셰이더가 없습니다.");
        return nullptr;
    }
    pipelineStateDesc.VS.pShaderBytecode = vsIt->second->GetBufferPointer();
    pipelineStateDesc.VS.BytecodeLength = vsIt->second->GetBufferSize();

    // PS
    std::unordered_map<Render::EShaderStage, Microsoft::WRL::ComPtr<ID3DBlob>>::const_iterator psIt =
        shaderTable.find(Render::EShaderStage::ePixel);
    if (psIt != shaderTable.cend())
    {
        pipelineStateDesc.PS.pShaderBytecode = psIt->second->GetBufferPointer();
        pipelineStateDesc.PS.BytecodeLength = psIt->second->GetBufferSize();
    }

    // GS
    std::unordered_map<Render::EShaderStage, Microsoft::WRL::ComPtr<ID3DBlob>>::const_iterator gsIt =
        shaderTable.find(Render::EShaderStage::eGeometry);
    if (gsIt != shaderTable.cend())
    {
        pipelineStateDesc.GS.pShaderBytecode = gsIt->second->GetBufferPointer();
        pipelineStateDesc.GS.BytecodeLength = gsIt->second->GetBufferSize();
    }

    // DS
    std::unordered_map<Render::EShaderStage, Microsoft::WRL::ComPtr<ID3DBlob>>::const_iterator dsIt =
        shaderTable.find(Render::EShaderStage::eDomain);
    if (dsIt != shaderTable.cend())
    {
        pipelineStateDesc.DS.pShaderBytecode = dsIt->second->GetBufferPointer();
        pipelineStateDesc.DS.BytecodeLength = dsIt->second->GetBufferSize();
    }

    // HS
    std::unordered_map<Render::EShaderStage, Microsoft::WRL::ComPtr<ID3DBlob>>::const_iterator hsIt =
        shaderTable.find(Render::EShaderStage::eHull);
    if (hsIt != shaderTable.cend())
    {
        pipelineStateDesc.HS.pShaderBytecode = hsIt->second->GetBufferPointer();
        pipelineStateDesc.HS.BytecodeLength = hsIt->second->GetBufferSize();
    }

    ID3D12PipelineState *pipelineState = nullptr;
    HRESULT ret = mDevice->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState));
    if (FAILED(ret))
    {

        LOG_MESSAGE_ERROR("MaterialManager", "파이프라인 생성실패");
    }

    return pipelineState;
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE D3DRender::D3DMaterialManager::GetPrimitiveTopologyType(
    Render::EInputLayoutType inputLayoutType)
{

    if (inputLayoutType == Render::EInputLayoutType::eLine)
    {
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    }
    else if (inputLayoutType == Render::EInputLayoutType::eBillboard)
    { // 빌보드
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    }
    else if (inputLayoutType == Render::EInputLayoutType::eStaticMesh)
    {
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    }
    else if (inputLayoutType == Render::EInputLayoutType::eSkinningMesh)
    {
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    }

    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
}

// bool D3DRender::D3DMaterialManager::CreateMaterialConstantBuffer(const Render::CreationMaterialInfo&
// creationMaterialInfo, D3DMaterialItem& oMaterialItem)
//{
//
//
//	GRM::BufferDesc bufferDesc;
//	bufferDesc.mBufferUsage = GRM::EBufferUsage::eConstantBuffer;
//	bufferDesc.mBufferMemoryAccess = GRM::EBufferMemoryAccess::eCpuWriteOnly;
//	bufferDesc.mElementDataNum = 1;
//	bufferDesc.mElementDataSize = 0; //sizeof(머터리얼구조체크기);
//	bufferDesc.mBufferSize = 0; //sizeof(머터리얼구조체크기);
//	GRM::GRMPtr bufferPtr = mGpuResourceManager->CreateBuffer(bufferDesc);
//
//
//	if (bufferPtr.getResource() == nullptr)
//	{	//log
//		return false;
//	}
//	D3DGRM::D3DGpuBuffer* d3dbuffer = static_cast<D3DGRM::D3DGpuBuffer*>(bufferPtr.getResource());
//
//	oMaterialItem.mMaterialConstantBuffer = bufferPtr;
//
//	return true;
// }

Render::MaterialID D3DRender::D3DMaterialManager::GetNextMaterialID() const
{

    if (mFreeMaterialIDVector.empty())
        return ++mNextMaterialID;

    Render::MaterialID id = mFreeMaterialIDVector.back();
    mFreeMaterialIDVector.pop_back();

    return id;
}

void D3DRender::D3DMaterialManager::InitD3DBlendTable()
{

    mD3DBlendTable["BLEND_ZERO"] = D3D12_BLEND_ZERO;
    mD3DBlendTable["BLEND_ONE"] = D3D12_BLEND_ONE;
    mD3DBlendTable["BLEND_SRC_COLOR"] = D3D12_BLEND_SRC_COLOR;
    mD3DBlendTable["BLEND_INV_SRC_COLOR"] = D3D12_BLEND_INV_SRC_COLOR;
    mD3DBlendTable["BLEND_SRC_ALPHA"] = D3D12_BLEND_SRC_ALPHA;
    mD3DBlendTable["BLEND_INV_SRC_ALPHA"] = D3D12_BLEND_INV_SRC_ALPHA;
    mD3DBlendTable["BLEND_DEST_ALPHA"] = D3D12_BLEND_DEST_ALPHA;
    mD3DBlendTable["BLEND_INV_DEST_ALPHA"] = D3D12_BLEND_INV_DEST_ALPHA;
    mD3DBlendTable["BLEND_DEST_COLOR"] = D3D12_BLEND_DEST_COLOR;
    mD3DBlendTable["BLEND_INV_DEST_COLOR"] = D3D12_BLEND_INV_DEST_COLOR;
    mD3DBlendTable["BLEND_SRC_ALPHA_SAT"] = D3D12_BLEND_SRC_ALPHA_SAT;
    mD3DBlendTable["BLEND_BLEND_FACTOR"] = D3D12_BLEND_BLEND_FACTOR;
    mD3DBlendTable["BLEND_INV_BLEND_FACTOR"] = D3D12_BLEND_INV_BLEND_FACTOR;
    mD3DBlendTable["BLEND_SRC1_COLOR"] = D3D12_BLEND_SRC1_COLOR;
    mD3DBlendTable["BLEND_INV_SRC1_COLOR"] = D3D12_BLEND_INV_SRC1_COLOR;
    mD3DBlendTable["BLEND_SRC1_ALPHA"] = D3D12_BLEND_SRC1_ALPHA;
    mD3DBlendTable["BLEND_INV_SRC1_ALPHA"] = D3D12_BLEND_INV_SRC1_ALPHA;
    mD3DBlendTable["ALPHA_FACTOR"] = D3D12_BLEND_ALPHA_FACTOR;
    mD3DBlendTable["INV_ALPHA_FACTOR"] = D3D12_BLEND_INV_ALPHA_FACTOR;
}

void D3DRender::D3DMaterialManager::InitD3DBlendOpTable()
{

    mD3DBlendOpTable["ADD"] = D3D12_BLEND_OP_ADD;
    mD3DBlendOpTable["SUBTRACT"] = D3D12_BLEND_OP_SUBTRACT;
    mD3DBlendOpTable["REV_SUBTRACT"] = D3D12_BLEND_OP_REV_SUBTRACT;
    mD3DBlendOpTable["MIN"] = D3D12_BLEND_OP_MIN;
    mD3DBlendOpTable["MAX"] = D3D12_BLEND_OP_MAX;
}
