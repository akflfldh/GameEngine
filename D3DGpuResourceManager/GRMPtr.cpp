#include "D3DGpuResourceManager/GRMPtr.h"
#include "D3DGpuResourceManager/IGpuResource.h"
#include "D3DGpuResourceManager/IGpuResourceManager.h"

GRM::GRMPtr::GRMPtr() : mControlBlock(nullptr) {}

GRM::GRMPtr::GRMPtr(IGpuResource *gpuResource) : mControlBlock(new GRM::GRMPtrControlBlcok)
{

    mControlBlock->mGpuResource = gpuResource;
    mControlBlock->mRefCount = 1;
    //	mControlBlock->mGpuResourceManager = IGpuResourceManager::GetInstance();
}

GRM::GRMPtr::~GRMPtr()
{
    if (mControlBlock)
    {
        if (mControlBlock->mRefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            Release();
        }
    }
}

GRM::GRMPtr::GRMPtr(const GRMPtr &ptr)
{
    mControlBlock = ptr.mControlBlock;
    if (mControlBlock)
        mControlBlock->mRefCount.fetch_add(1, std::memory_order_relaxed);
}

GRM::GRMPtr &GRM::GRMPtr::operator=(const GRMPtr &ptr)
{

    if (this == &ptr)
        return *this;

    // 스레드에 안전하게
    // 기존 리소스 release

    if (mControlBlock)
    {

        if (mControlBlock->mRefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            // 해제요청
            Release();
        }
    }
    // 새로운리소스
    mControlBlock = ptr.mControlBlock;
    if (mControlBlock)
        mControlBlock->mRefCount.fetch_add(1, std::memory_order_relaxed);

    return *this;
}

GRM::GRMPtr::GRMPtr(GRMPtr &&rhs)
{
    mControlBlock = rhs.mControlBlock;
    rhs.mControlBlock = nullptr;
}

GRM::GRMPtr &GRM::GRMPtr::operator=(GRMPtr &&rhs)
{

    if (this == &rhs)
        return *this;

    // 스레드 안전필요
    // 기존 controlblock처리

    if (mControlBlock)
    {
        if (mControlBlock->mRefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            Release();
        }
    }

    mControlBlock = rhs.mControlBlock;
    rhs.mControlBlock = nullptr;

    return *this;
}

GRM::IGpuResource *GRM::GRMPtr::getResource() const
{

    if (mControlBlock)
        return mControlBlock->mGpuResource;

    return nullptr;
}

void GRM::GRMPtr::Release()
{

    if (mControlBlock)
    {
        // 스레드안전하게
        IGpuResourceManager::GetInstance()->Release(mControlBlock->mGpuResource); // manager에게 release를 요청함으로서
        // 가비지 컬렉터에 들어가도록한다.
        delete mControlBlock;
        mControlBlock = nullptr;
    }
}

GRM::GRMPtr GRM::MakeGRMPtr(GRM::IGpuResource *gpuResource)
{
    return GRMPtr(gpuResource);
}
