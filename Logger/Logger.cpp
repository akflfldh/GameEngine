#include "Logger/Logger.h"
#include "Logger/LoggerImpl.h"
QuadLog::Logger *QuadLog::Logger::GetInstance()
{
    static QuadLog::LoggerImpl instance;
    return &instance;
}

QuadLog::Logger::Logger() {}

QuadLog::Logger::~Logger() {}

void QuadLog::Logger::Log(ELogLevel logLevel, const char *category, const std::string &message)
{
    Log(logLevel, category, message.c_str());
}

void QuadLog::Logger::Log(const char *logLevel, const char *category, const std::string &message)
{
    Log(logLevel, category, message.c_str());
}

void QuadLog::Logger::LogInfo(const char *category, const std::string &message)
{
    LogInfo(category, message.c_str());
}

void QuadLog::Logger::LogWarning(const char *category, const std::string &message)
{
    LogWarning(category, message.c_str());
}

void QuadLog::Logger::LogError(const char *category, const std::string &message)
{
    LogError(category, message.c_str());
}

void QuadLog::Logger::LogCritical(const char *category, const std::string &message)
{
    LogCritical(category, message.c_str());
}
