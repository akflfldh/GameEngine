#pragma once

#include <CommonHeader/GpuTypes.h>
#include <D3DGpuResourceManager/GRMPtr.h>
#include <D3DGpuResourceManager/GpuResourceTypes.h>
#include <RenderFrontend/RenderFrontendType.h>
#include <RenderFrontend/RenderPass.h>
#include <RenderSystem/RenderType.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Render
{

class RenderPassGraphBuilder;
struct RenderPassNode;

struct FrameGraphResourceNode;
struct RenderPassSetUpData;
struct RenderResourceDesc;

struct FrameGraphResourceVersion
{
    int mVersionID = 0; // 디버깅용

    FrameGraphResourceNode *mOriginalResourceNode;
    /// ResourceDescription Desc;
    // 이 리소스를 '쓰는(생산하는)' 패스 (단 하나만 존재)
    RenderPassNode *pProducer = nullptr;

    // 이 리소스를 '읽는(소비하는)' 패스들의 목록
    std::vector<RenderPassNode *> Consumers;

    // 이  리소스를 write 한후의 상태
    EResourceState mState;
};

// 각 리소스의 상태를 추적하는 노드
struct FrameGraphResourceNode
{

    std::string Name;

    std::vector<std::unique_ptr<FrameGraphResourceVersion>> mResourceVersionList;

    // 초기상태!
    EResourceState mInitState;
    EResourceState mLastState;
};

struct ResourceUsage
{
    FrameGraphResourceVersion *mVersion;
    EResourceState mRequiredState;
};

// struct ResourceBarrier
//{
//     void *mResource;
//     EResourceState mBeforeState;
//     EResourceState mAfterState;
// };

struct RenderTargetResourceInfo
{
    FrameGraphResourceVersion *mFrameGraphResourceVersion = nullptr; // nullptr이라면 사용하지않음
    bool bClear = false;
    float mClearColor[4] = {0, 0, 0, 0};
};

struct DepthStencilResourceInfo
{
    FrameGraphResourceVersion *mFrameGraphResourceVersion = nullptr; // nullptr이라면 사용하지않음
    bool bClear = false;
    float mClearValue = 1.0f;
};

struct RenderPassNode
{
    std::string mPassName;
    std::unique_ptr<Render::IRenderPass> mRenderPass;
    std::function<void(const RenderPassExecuteContext &)> executeCallback;

    std::unordered_set<RenderPassNode *> mSuccessorRenderPassNodeList;
    std::unordered_set<RenderPassNode *> mPrepredecessorRenderPassNodeList;

    // 위상정렬구축을 위한 진입차수
    int mInDegree;

    // read하는 input resource list
    std::vector<ResourceUsage> mInputLists;
    // wrtie 하는 output resource
    std::vector<ResourceUsage> mOutputLists;

    // 이 패스 실행전 수행해야할 리소스 배리어
    std::vector<ResourceBarrier> PreBarrier;

    // 해당 패스의 렌더타켓정보(일단 하나만 사용한다고 가정하고)
    RenderTargetResourceInfo mRenderTargetInfo;
    DepthStencilResourceInfo mDepthStencilInfo;
};

class RenderPassGraph
{
    friend class RenderPassGraphBuilder;
    using RenderPassCallbackTable =
        std::unordered_map<std::string, std::pair<std::function<void(RenderPassGraphBuilder &)>,
                                                  std::function<void(const RenderPassExecuteContext &)>>>;

    using RenderPassCallbackList = std::vector<std::pair<std::function<void(RenderPassGraphBuilder &)>,
                                                         std::function<void(const RenderPassExecuteContext &)>>>;

  public:
    static RenderPassGraph *GetInstance();
    RenderPassGraph();
    ~RenderPassGraph();

    // RenderPass들이 Register된후 호출
    void RegisterRenderPassCallback(const std::string &renderPassName,
                                    std::function<void(RenderPassGraphBuilder &builder)> setUpCallback,
                                    std::function<void(const RenderPassExecuteContext &)> executeCallback);

    // 외부에서 호출
    void RegisterRenderPass(std::unique_ptr<Render::IRenderPass> &&renderPass, const std::string &renderPassName,
                            const RenderPassSetUpData &passSetUpData);

    GRM::GRMPtr GetTexture(const std::string &texName) const;

    // 렌더파이프라인들의 렌더패스들의 진행 그래프구축
    void Compile();

    void Execute(const RenderPassExecuteContext &renderPassExecuteContext);

    // 매프레임 재사용을위해 reset
    // tex resource는 유지된다.
    void Reset();

    // 외부에서 생성된 tex 등록
    void Import(const std::string &texName, GRM::GRMPtr ptr, EResourceState initState);

    void SetFrameCount(uint64_t count);
    void SetFenceValue(uint64_t value);

  private:
    // 새로운 tex생성해서 write
    void Write(const std::string &texName, const GRM::TextureDesc &texDesc);

    // 새로운 tex생성 (실제는 생성일수도 재사용일수도)
    void Create(const std::string &texName, const RenderResourceDesc &Desc, EResourceState initState);

    // 이미 존재하는 tex에 write

    // write의 한종류이지만 특별(렌더타켓은 반드시 이 메서드를 사용)
    // color : float[4] 배열
    void SetRenderTarget(const std::string &texName, const std::string &passName, bool bClear, const float *color);
    void SetDepthStencil(const std::string &texName, const std::string &passName, bool bClear, float clearValue,
                         bool bDepthWrite);

    // resourceState : 어떤상태로 write할지
    void Write(const std::string &texName, const std::string &passName, EResourceState resourceState);

    // resourceState :  어떤상태로 read 할지
    void Read(const std::string &texName, const std::string &passName, EResourceState resourceState);

    void AllocResourceAll();

    void UploadMaterialData(const RenderPassExecuteContext &renderPassExecuteContext);
    void UploadLightData(const RenderPassExecuteContext &renderPassExecuteContext);

  private:
    // 렌더 pass들에서 렌더타켓,셰이더리소스로 사용되는 텍스처테이블
    std::unordered_map<std::string, std::pair<GRM::GRMPtr, Render::RenderResourceDesc>> mTexResourceTable;
    RenderPassCallbackTable mRenderPassCallbackTable;
    RenderPassCallbackList mRenderPassCallbackList;

    // 의존성그래프 구축을위한 리소스 사용표시 테이블
    std::unordered_map<std::string, std::unique_ptr<FrameGraphResourceNode>> mResourceNodeTable;
    std::unordered_map<std::string, RenderPassNode *> mRenderPassNodeTable;

    std::vector<std::unique_ptr<RenderPassNode>> mRenderPassNodeList;

    // 위상정렬된 렌더패스 리스트 , 순차적으로 실행
    std::vector<RenderPassNode *> mOrderedRenderPassNodeList;

    // 현재 Compile중인 renderPassName
    std::string mCurrPassName;

    // 사용하는 resource 리스트 , - reset시 반납한다.
    std::vector<PooledRenderResource *> mAllocatedResources;

    uint64_t mFrameCount;
    uint64_t mFenceValue;
};

class RenderPassGraphBuilder
{
  public:
    RenderPassGraphBuilder(RenderPassGraph *renderPassGraph);
    ~RenderPassGraphBuilder();

    void Create(const std::string &texName, const RenderResourceDesc &Desc, EResourceState initState);

    // rendertarget set , 렌더타켓은 write 메서드말고 이 메서드를 호출
    void SetRenderTarget(const std::string &texName, const std::string &passName, bool bClear, const float *color);

    void SetDepthStencil(const std::string &texName, const std::string &passName, bool bClear, float clearValue,
                         bool bDepthWrite);

    // resourceState : 어떤상태로 write할지
    void Write(const std::string &texName, const std::string &passName, EResourceState resourceState);
    // resourceState :  어떤상태로 read 할지
    void Read(const std::string &texName, const std::string &passName, EResourceState resourceState);

  private:
    RenderPassGraph *mRenderPassGraph;
};

} // namespace Render