#pragma once

#include <CoreBase/CoreBaseDllMacro.h>
#include <memory>
#include <mutex>
#include <string>

struct TaskContext
{
    std::mutex mMutex;
    std::string mMessage;
};

class COREBASE_API PrograssContext
{

  public:
    PrograssContext(std::shared_ptr<TaskContext> taskContext);
    ~PrograssContext();

    void Report(const std::string &message);

  private:
    std::shared_ptr<TaskContext> mTaskContext;
};