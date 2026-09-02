#include "PrograssContext.h"

PrograssContext::PrograssContext(std::shared_ptr<TaskContext> taskContext) : mTaskContext(taskContext) {}

PrograssContext::~PrograssContext() {}

void PrograssContext::Report(const std::string &message)
{

    if (mTaskContext)
    {
        std::lock_guard lock(mTaskContext->mMutex);
        mTaskContext->mMessage = message;
    }
}
