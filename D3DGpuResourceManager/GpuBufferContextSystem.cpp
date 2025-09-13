#include "D3DGpuResourceManager/GpuBufferContextSystem.h"
#include "D3DGpuResourceManager/GpuBufferContextSystemImpl.h"

GRM::GpuBufferContextSystem *GRM::GpuBufferContextSystem::GetInstance()
{
    static GpuBufferContextSystemImpl instance;
    return &instance;
}

GRM::GpuBufferContextSystem::~GpuBufferContextSystem() {}

GRM::GpuBufferContextSystem::GpuBufferContextSystem() {}
