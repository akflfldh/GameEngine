#include "Logger/LoggerImpl.h"

bool QuadLog::LoggerImpl::SetLoggerFile(const char *outputFile)
{

    if (fout.is_open())
        fout.close();

    fout.open(outputFile);
    if (fout.is_open() == false)
        return false;

    unsigned char utf8BOM[] = {0xEF, 0xBB, 0xBF};
    fout.write(reinterpret_cast<const char *>(utf8BOM), sizeof(utf8BOM));

    mOutputFile = outputFile;
    mFileActiveFlag = true;

    return true;
}

void QuadLog::LoggerImpl::Log(ELogLevel logLevel, const char *category, const char *message)
{

    if (mMinLogLevel > logLevel)
        return;

    std::string logMessage = BuildLogMessage(logLevel, category, message);

    {
        std::lock_guard<std::mutex> lock_guard(mLogMutex);

        if (mFileActiveFlag)
        {
            fout << logMessage;
        }
        fout.flush();

        mCallbackSystem.Call(logMessage.c_str());
    }
}

void QuadLog::LoggerImpl::Log(const char *logLevel, const char *category, const char *message)
{

    ELogLevel logLevelEnum = ParseLogLevel(logLevel);

    if ((int)logLevelEnum == -1)
        return;

    Log(logLevelEnum, category, message);
}

void QuadLog::LoggerImpl::LogInfo(const char *category, const char *message)
{
    Log(ELogLevel::eInfo, category, message);
}

void QuadLog::LoggerImpl::LogWarning(const char *category, const char *message)
{
    Log(ELogLevel::eWarning, category, message);
}

void QuadLog::LoggerImpl::LogError(const char *category, const char *message)
{
    Log(ELogLevel::eError, category, message);
}

void QuadLog::LoggerImpl::LogCritical(const char *category, const char *message)
{
    Log(ELogLevel::eCritical, category, message);
}

void QuadLog::LoggerImpl::OnActive(int filterID)
{
    if (filterID == 0)
    {
        mFileActiveFlag = true;
        return;
    }
}

void QuadLog::LoggerImpl::OffActive(int filterID)
{
    if (filterID == 0)
    {
        mFileActiveFlag = false;
        return;
    }
}

void QuadLog::LoggerImpl::SetMinLogLevel(ELogLevel minLogLevel)
{

    mMinLogLevel = minLogLevel;
}

QuadLog::LoggerImpl::LoggerImpl() : Logger(), mFileActiveFlag(false), mNextID(1)
{
    mLogLevelStringTable[ELogLevel::eWarning] = "[Warning]";
    mLogLevelStringTable[ELogLevel::eError] = "[Error]";
    mLogLevelStringTable[ELogLevel::eCritical] = "[Critical]";
    mLogLevelStringTable[ELogLevel::eInfo] = "[Info]";

    mLogLevelEnumTable["Warning"] = ELogLevel::eWarning;
    mLogLevelEnumTable["Error"] = ELogLevel::eError;
    mLogLevelEnumTable["Critical"] = ELogLevel::eCritical;
    mLogLevelEnumTable["Info"] = ELogLevel::eInfo;

    mMinLogLevel = ELogLevel::eInfo;
}

QuadLog::LoggerImpl::~LoggerImpl() {}

std::string QuadLog::LoggerImpl::BuildLogMessage(ELogLevel logLevel, const char *category, const char *message)
{

    std::string log = mLogLevelStringTable[logLevel] + "[" + category + "] : " + message + "\n";
    return log;
}

QuadLog::ELogLevel QuadLog::LoggerImpl::ParseLogLevel(const char *logLevelStr) const
{

    std::unordered_map<std::string, ELogLevel>::const_iterator it = mLogLevelEnumTable.find(logLevelStr);

    if (it == mLogLevelEnumTable.end())
        return (ELogLevel)-1;

    return it->second;
}
