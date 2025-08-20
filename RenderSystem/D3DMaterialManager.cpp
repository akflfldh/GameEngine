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
}

D3DRender::D3DMaterialManager::~D3DMaterialManager() {}

Render::MaterialID D3DRender::D3DMaterialManager::CreateMaterial(
    const Render::CreationMaterialInfo &creationMaterialInfo)
{

    // 맨처음 중복확인
    // creationMaterialInfo.mName; 할수도있고 필요없을수도

    D3DMaterialItem materialItem;
    // 세이더에서 사용하는 리소스들에 대한정보를 정리한다.
    const std::vector<Render::TextureShaderResourceInfo> &textureShaderResourceInfo =
        creationMaterialInfo.mShaderResourceInfoSet.mTextureShaderResourceInfoVector;

    // 일단 이렇게 복사하는것만으로도 shaderResource들의 정보가 유지된다.
    materialItem.mShaderResourceInfoSet = creationMaterialInfo.mShaderResourceInfoSet;

    bool ret = BuildMainPass(creationMaterialInfo, materialItem.mMainPass, materialItem.mShaderResourceInfoSet);
    if (ret == false)
    {
        return MaterialIDNone;
    }

    // 향후 확장
    // engine default shader.
    // post shdader 를 처리한다.(리플렉션 시스템도 가동한다)

    Render::MaterialID id = GetNextMaterialID();
    mMaterialItemTable[id] = std::make_unique<D3DRender::D3DMaterialItem>(std::move(materialItem));
    return id;

    return 0;
}

D3DRender::D3DMaterialItem *D3DRender::D3DMaterialManager::GetMaterialItem(Render::MaterialID materialID) const
{

    std::unordered_map<Render::MaterialID, std::unique_ptr<D3DMaterialItem>>::const_iterator it =
        mMaterialItemTable.find(materialID);

    if (it == mMaterialItemTable.cend())
        return nullptr;

    return it->second.get();
}

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
                    std::find_if(oShaderResourceInfo.mBufferShaderResourceInfoVector.begin(),
                                 oShaderResourceInfo.mBufferShaderResourceInfoVector.end(),
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

            std::vector<Render::BufferShaderResourceInfo>::const_iterator it =
                std::find_if(resourceInfoSet.mBufferShaderResourceInfoVector.cbegin(),
                             resourceInfoSet.mBufferShaderResourceInfoVector.cend(),
                             [&resourceName](const Render::BufferShaderResourceInfo &bufferInfo)
                             {
                                 if (bufferInfo.mName == resourceName)
                                 {
                                     return true;
                                 }

                                 return false;
                             });
            // 일치하는 이름의 리소스가 명시되어있지않다면 실패
            if (it == resourceInfoSet.mBufferShaderResourceInfoVector.cend())
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

            std::vector<Render::TextureShaderResourceInfo>::const_iterator it =
                std::find_if(resourceInfoSet.mTextureShaderResourceInfoVector.cbegin(),
                             resourceInfoSet.mTextureShaderResourceInfoVector.cend(),
                             [&resourceName](const Render::TextureShaderResourceInfo &bufferInfo)
                             {
                                 if (bufferInfo.mName == resourceName)
                                 {
                                     return true;
                                 }

                                 return false;
                             });
            // 일치하는 이름의 리소스가 명시되어있지않다면 실패
            if (it == resourceInfoSet.mTextureShaderResourceInfoVector.cend())
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
    pipelineStateDesc.SampleMask = 0xFFFFFFFF;
    pipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = ConvertToBlendEnableFlag(mainRenderPass.mBlendMode);
    pipelineStateDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    pipelineStateDesc.DepthStencilState.DepthFunc = ConvertToD3DCmpFun(mainRenderPass.mDepthCompareMode);
    pipelineStateDesc.DepthStencilState.DepthEnable = ConvertToDepthWriteModeEnableFlag(mainRenderPass.mDepthWriteMode);

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
    if (psIt == compiledShaderTable.cend())
    {
        LOG_MESSAGE_ERROR("MaterialManager", "컴파일된 픽셀셰이더가 없습니다.");
        return nullptr;
    }
    pipelineStateDesc.PS.pShaderBytecode = psIt->second->GetBufferPointer();
    pipelineStateDesc.PS.BytecodeLength = psIt->second->GetBufferSize();

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

        oInputElementDescVector.push_back({"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back({"TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back({"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    }
    if (mainRenderPass.mInputType >= 1)
    {
        oInputElementDescVector.push_back({"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back({"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        oInputElementDescVector.push_back({"TEX", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

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

// bool D3DRender::D3DMaterialManager::LoadShaderFile(const Render::ShaderSourceInfo& shaderInfo, char* oBuffer ,size_t
// & oBufferSize)
//{
//
//	std::ifstream fin(shaderInfo.mSourceFilePath);
//	if (!fin.is_open())
//	{
//		//log
//		return false;
//	}
//
//
//	fin.seekg(std::ios::end);
//	size_t fileSize = fin.tellg();
//
//	fin.read(oBuffer, fileSize);
//	oBufferSize = fileSize;
//	return true;
// }
//
// bool D3DRender::D3DMaterialManager::CompileShader(const Render::ShaderSourceInfo& shaderInfo,
//	const char* buffer,size_t bufferSize, D3D12_GRAPHICS_PIPELINE_STATE_DESC& oPipelineStateDesc)
//{
//	//이 함수 더 자세히 다룰것 (특히 매개변수들)
//	Microsoft::WRL::ComPtr<ID3DBlob> compiledBlob = nullptr;
//	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
//	HRESULT result =D3DCompile(buffer, bufferSize, nullptr, nullptr, nullptr, shaderInfo.mEntryPoint.c_str(),
// shaderInfo.mTarget.c_str(), 		D3DCOMPILE_DEBUG, 0, &compiledBlob, &errorBlob);
//
//	if (result != S_OK)
//	{
//		//errorBlob
//		errorBlob->GetBufferPointer();
//		return false;
//	}
//
//	//D3DCompileFromFile()
//
//
//	SetShaderToPipeline(shaderInfo, compiledBlob, oPipelineStateDesc);
//
//	return true;
// }
//
// void D3DRender::D3DMaterialManager::SetShaderToPipeline(const Render::ShaderSourceInfo& shaderInfo,
//	Microsoft::WRL::ComPtr<ID3DBlob> compiledBlob, D3D12_GRAPHICS_PIPELINE_STATE_DESC& oPipelineStateDesc)
//{
//	switch (shaderInfo.mStage)
//	{
//	case Render::EShaderStage::eVertex:
//
//		oPipelineStateDesc.VS.pShaderBytecode = compiledBlob->GetBufferPointer();
//		oPipelineStateDesc.VS.BytecodeLength = compiledBlob->GetBufferSize();
//		break;
//	case Render::EShaderStage::ePixel:
//		oPipelineStateDesc.PS.pShaderBytecode = compiledBlob->GetBufferPointer();
//		oPipelineStateDesc.PS.BytecodeLength = compiledBlob->GetBufferSize();
//
//		break;
//
//	case Render::EShaderStage::eGeometry:
//		oPipelineStateDesc.GS.pShaderBytecode = compiledBlob->GetBufferPointer();
//		oPipelineStateDesc.GS.BytecodeLength = compiledBlob->GetBufferSize();
//
//		break;
//
//	case Render::EShaderStage::eDomain:
//		oPipelineStateDesc.DS.pShaderBytecode = compiledBlob->GetBufferPointer();
//		oPipelineStateDesc.DS.BytecodeLength = compiledBlob->GetBufferSize();
//
//		break;
//
//
//	case Render::EShaderStage::eHull:
//
//		oPipelineStateDesc.HS.pShaderBytecode = compiledBlob->GetBufferPointer();
//		oPipelineStateDesc.HS.BytecodeLength = compiledBlob->GetBufferSize();
//		break;
//	}
//
//	return;
//
//
// }
//
// void D3DRender::D3DMaterialManager::SetInputElement(D3D12_GRAPHICS_PIPELINE_STATE_DESC& oPipelineStateDesc,
//	std::vector<D3D12_INPUT_ELEMENT_DESC> & oInputElementDescVector)
//{
//	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
//
//	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescVector = {
//		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//
//		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//
//		{"TEX",0,DXGI_FORMAT_R32G32_FLOAT,0,24,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//	};
//
//	oInputElementDescVector = std::move(inputElementDescVector);
//
//	inputLayoutDesc.NumElements = (UINT)oInputElementDescVector.size();
//	inputLayoutDesc.pInputElementDescs = inputElementDescVector.data();
//	oPipelineStateDesc.InputLayout = inputLayoutDesc;
//
// }
//
// void D3DRender::D3DMaterialManager::SetTextureResource(const Render::CreationMaterialInfo& creationMaterialInfo,
// D3DMaterialItem& oMaterialItem)
//{
//
//	for (auto& textureInfoElement : creationMaterialInfo.mMaterialTextureInfo)
//	{
//		D3DGRM::D3DGpuTexture* texture = static_cast<D3DGRM::D3DGpuTexture*>(textureInfoElement.mTextureResource);
//
//		oMaterialItem.mMaterialTextureTable[textureInfoElement.mType] = texture;
//		/*switch (textureInfoElement.mType)
//		{
//		case Render::MaterialTextureInfo::Type::Albedo:
//			oMaterialItem.mMaterialTextureTable[textureInfoElement.mType] = texture;
//
//
//			break;
//
//		case Render::MaterialTextureInfo::Type::Emissive:
//
//
//			break;
//
//		case Render::MaterialTextureInfo::Type::MetallicRoughness:
//
//
//			break;
//
//		case Render::MaterialTextureInfo::Type::Normal:
//
//
//
//			break;
//
//		}*/
//
//
//	}
//
//
//
//
//
// }
//
// bool D3DRender::D3DMaterialManager::CreateRootSignature(const Render::CreationMaterialInfo& creationMaterialInfo,
// D3DMaterialItem& oMaterialItem, D3D12_GRAPHICS_PIPELINE_STATE_DESC& oPipelineStateDesc)
//{
//
//
//	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
//	//항상 고정된 개수, 고정된 위치 , 고정된 역할,고정된 레지스터
//
//	//std::vector<D3D12_ROOT_PARAMETER> rootParameterDescVector(5);
//	std::vector<CD3DX12_ROOT_PARAMETER> rootParameterDescVector(5);
//	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRangeVector(5);
//
//
//	//pass constant - 루트매개변수 0 b0
//	rootParameterDescVector[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	rootParameterDescVector[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
//	rootParameterDescVector[0].DescriptorTable.NumDescriptorRanges = 1;
//	descriptorRangeVector[0].BaseShaderRegister = 0;
//	descriptorRangeVector[0].NumDescriptors = 1;
//	descriptorRangeVector[0].OffsetInDescriptorsFromTableStart = 0;
//	descriptorRangeVector[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
//	descriptorRangeVector[0].RegisterSpace = 0;
//	rootParameterDescVector[0].DescriptorTable.pDescriptorRanges = &descriptorRangeVector[0];
//
//
//	//object structured  -루트매개변수 1 t0
//
//	rootParameterDescVector[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	rootParameterDescVector[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
//	rootParameterDescVector[1].DescriptorTable.NumDescriptorRanges = 1;
//	descriptorRangeVector[1].BaseShaderRegister = 0;
//	descriptorRangeVector[1].NumDescriptors = 1;
//	descriptorRangeVector[1].OffsetInDescriptorsFromTableStart = 0;
//	descriptorRangeVector[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
//	descriptorRangeVector[1].RegisterSpace = 0;
//	rootParameterDescVector[1].DescriptorTable.pDescriptorRanges = &descriptorRangeVector[1];
//
//
//	//material constant	 - 루트매개변수2  b1
//
//	rootParameterDescVector[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	rootParameterDescVector[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
//	rootParameterDescVector[2].DescriptorTable.NumDescriptorRanges = 1;
//	descriptorRangeVector[2].BaseShaderRegister = 1;
//	descriptorRangeVector[2].NumDescriptors = 1;
//	descriptorRangeVector[2].OffsetInDescriptorsFromTableStart = 0;
//	descriptorRangeVector[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
//	descriptorRangeVector[2].RegisterSpace = 0;
//	rootParameterDescVector[2].DescriptorTable.pDescriptorRanges = &descriptorRangeVector[2];
//
//
//
//
//
//	//texture (diffuse) -루트매개변수3 t1
//
//	rootParameterDescVector[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	rootParameterDescVector[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//	rootParameterDescVector[3].DescriptorTable.NumDescriptorRanges = 1;
//	descriptorRangeVector[3].BaseShaderRegister = 1;
//	descriptorRangeVector[3].NumDescriptors = 1;
//	descriptorRangeVector[3].OffsetInDescriptorsFromTableStart = 0;
//	descriptorRangeVector[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
//	descriptorRangeVector[3].RegisterSpace = 0;
//	rootParameterDescVector[3].DescriptorTable.pDescriptorRanges = &descriptorRangeVector[3];
//
//
//
//
//
//	//sampler		-루트매개변수 4 s0
//
//	rootParameterDescVector[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	rootParameterDescVector[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//	rootParameterDescVector[4].DescriptorTable.NumDescriptorRanges = 1;
//	descriptorRangeVector[4].BaseShaderRegister = 0;
//	descriptorRangeVector[4].NumDescriptors = 1;
//	descriptorRangeVector[4].OffsetInDescriptorsFromTableStart = 0;
//	descriptorRangeVector[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
//	descriptorRangeVector[4].RegisterSpace = 0;
//	rootParameterDescVector[4].DescriptorTable.pDescriptorRanges = &descriptorRangeVector[4];
//
//	rootSignatureDesc.NumParameters =(UINT)rootParameterDescVector.size();
//	rootSignatureDesc.NumStaticSamplers = 0;
//	rootSignatureDesc.pParameters = rootParameterDescVector.data();
//	rootSignatureDesc.pStaticSamplers = nullptr;
//	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//
//	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSignatureBlob = nullptr;
//	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
//	HRESULT result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
//&serializedRootSignatureBlob, &errorBlob);
//
//	if (result != S_OK)
//	{
//		//log
//		return false;
//	}
//
//	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
//	result =mDevice->CreateRootSignature(0, serializedRootSignatureBlob->GetBufferPointer(),
// serializedRootSignatureBlob->GetBufferSize(), 		IID_PPV_ARGS(&rootSignature));
//
//	if (result != S_OK)
//	{
//		//log
//		return false;
//	}
//
//	oMaterialItem.mRootSignature = rootSignature;
//	oPipelineStateDesc.pRootSignature = rootSignature.Get();
//
//	return true;
//
//
//
//
//
// }
//
// bool D3DRender::D3DMaterialManager::CreateGraphicsPipelineState(const Render::CreationMaterialInfo&
// creationMaterialInfo, D3DMaterialItem& oMaterialItem, D3D12_GRAPHICS_PIPELINE_STATE_DESC& oPipelineStateDesc)
//{
//	//blend
//
//	oPipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
//
//	oPipelineStateDesc.DepthStencilState= CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
//	oPipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
//	switch (creationMaterialInfo.mBlendMode)
//	{
//	case Render::EBlendMode::eOpaque:
//
//
//
//		break;
//	case Render::EBlendMode::eAlphaBlend:
//
//	//	oPipelineStateDesc.BlendState.AlphaToCoverageEnable = false;
//		//oPipelineStateDesc.BlendState.IndependentBlendEnable = false;//하나의렌더타켓만사용 지금은
//		//상세한설정을 제공하는것은 후에 진행
//		//지금은 고정된설정값으로
//		oPipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = true;
//		//oPipelineStateDesc.BlendState.RenderTarget[0].
//
//		break;
//	}
//
//
//	//cull
//	switch (creationMaterialInfo.mCullMode)
//	{
//	case Render::ECullMode::eNone:
//
//		oPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
//		break;
//
//	case Render::ECullMode::eFront:
//
//		oPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
//		break;
//
//
//	case Render::ECullMode::eBack:
//		oPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
//		break;
//	}
//
//	//depth test
//	switch (creationMaterialInfo.mDepthTestMode)
//	{
//	case Render::EDepthTestMode::eNone:
//
//		oPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
//
//		break;
//
//	case Render::EDepthTestMode::eLess:
//		oPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
//
//		break;
//
//	case Render::EDepthTestMode::eLessEqual:
//		oPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
//
//	case Render::EDepthTestMode::eEqual:
//		oPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;
//
//		break;
//	}
//
//	//depth write
//	switch (creationMaterialInfo.mDepthWriteMode)
//	{
//	case Render::EDepthWriteMode::eEnabled:
//		oPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
//
//		break;
//
//	case Render::EDepthWriteMode::eDisabled:
//		oPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
//		break;
//	}
//
//
//	//fill
//	switch (creationMaterialInfo.mFillMode)
//	{
//
//	case Render::EFillMode::eSolidMode:
//		oPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
//
//		break;
//
//	case Render::EFillMode::eWireFrameMode:
//
//		oPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
//		break;
//
//	}
//
//	//이둘은 사실상 고정
//	//머터리얼는 엔진에서 미리만들어논 불투명,투명 렌더패스에서 적용된것이기에
//	oPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
//	oPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
//
//	oPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; //나중에 옵션을받아서 설정
//	memset(&oPipelineStateDesc.StreamOutput, 0, sizeof(oPipelineStateDesc.StreamOutput));
//	oPipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
//	oPipelineStateDesc.NumRenderTargets = 1;
//	oPipelineStateDesc.CachedPSO.pCachedBlob = nullptr;
//	oPipelineStateDesc.CachedPSO.CachedBlobSizeInBytes = 0;
//	oPipelineStateDesc.NodeMask = 0;
//	oPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
//
//	oPipelineStateDesc.SampleDesc.Count = 1;
//	oPipelineStateDesc.SampleDesc.Quality = 0;
//
//
//
//	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
//	HRESULT result = mDevice->CreateGraphicsPipelineState(&oPipelineStateDesc, IID_PPV_ARGS(&pipelineState));
//	if (FAILED(result))
//	{
//		//log
//		//OutputDebugString()
//		return false;
//	}
//
//	oMaterialItem.mPSO = pipelineState;
//
//
//	return true;
// }

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
