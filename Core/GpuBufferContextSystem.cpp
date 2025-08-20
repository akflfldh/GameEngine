#include "Core/GpuBufferContextSystem.h"
#include "Core/GpuBufferContextSystemImpl.h"

Core::GpuBufferContextSystem *Core::GpuBufferContextSystem::GetInstance()
{
    static GpuBufferContextSystemImpl instance;
    return &instance;
}

Core::GpuBufferContextSystem::~GpuBufferContextSystem() {}

Core::GpuBufferContextSystem::GpuBufferContextSystem() {}
