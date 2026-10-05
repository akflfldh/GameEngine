#pragma once

#ifdef QUAD_LOG_EXPORT
#define LOGGER_API __declspec(dllexport)
#else

#define LOGGER_API __declspec(dllimport)
#endif

#include <algorithm>
#include <CoreBase/CallbackSystem.h>
#include <string>

#define LogLevelInfo "Info"
#define LogLevelWarning "Warning"
#define LogLevelError "Error"
#define LogLevelCritical "Critical"

#define LOG_MESSAGE(logLevel, category, message) QuadLog::Logger::GetInstance()->Log(logLevel, category, message)
#define LOG_MESSAGE_INFO(category, message) QuadLog::Logger::GetInstance()->LogInfo(category, message)
#define LOG_MESSAGE_WARNING(category, message) QuadLog::Logger::GetInstance()->LogWarning(category, message)
#define LOG_MESSAGE_ERROR(category, message) QuadLog::Logger::GetInstance()->LogError(category, message)
#define LOG_MESSAGE_CRITICAL(category, message) QuadLog::Logger::GetInstance()->LogCritical(category, message)

#include <filesystem>
namespace QuadLog
{

enum ELogLevel : int
{
    eInfo = 0, ///< 정보성 로그. 일반적인 진행 상황을 나타냅니다.
    eWarning,  ///< 경고성 로그. 잠재적인 문제를 나타내지만 에러는 아닙니다.
    eError,    ///< 오류 로그. 프로그램 실행에 영향을 미치는 오류를 나타냅니다.
    eCritical  ///< 치명적인 오류 로그. 프로그램 실행을 중단시킬 수 있는 심각한 오류입니다.
};

constexpr int kFileCallbackID = 0;

class LOGGER_API Logger
{
  public:
    static Logger *GetInstance();
    virtual ~Logger() = 0;

    // Initialize이다.
    virtual bool SetLoggerFile(const std::filesystem::path &outputFile) = 0;

    virtual void Log(ELogLevel logLevel, const char *category, const char *message) = 0;
    virtual void Log(const char *logLevel, const char *category, const char *message) = 0;
    virtual void LogInfo(const char *category, const char *message) = 0;
    virtual void LogWarning(const char *category, const char *message) = 0;
    virtual void LogError(const char *category, const char *message) = 0;
    virtual void LogCritical(const char *category, const char *message) = 0;

    // 문자열 편의 오버로드는 기존 가상 메서드로 전달하여 파일 출력과 콜백 동작을 유지한다.
    void Log(ELogLevel logLevel, const char *category, const std::string &message);
    void Log(const char *logLevel, const char *category, const std::string &message);
    void LogInfo(const char *category, const std::string &message);
    void LogWarning(const char *category, const std::string &message);
    void LogError(const char *category, const std::string &message);
    void LogCritical(const char *category, const std::string &message);

    virtual void OnActive(int filterID) = 0;
    virtual void OffActive(int filterID) = 0;

    virtual void SetMinLogLevel(ELogLevel minLogLevel) = 0;

    // 로그를 발생시킨 스레드에서 동기 호출한다. 전달된 문자열은 콜백 안에서 필요한 만큼 복사한다.
    // 등록·해제는 로그 호출과 겹치지 않아야 하며, 콜백에서 같은 Logger를 다시 호출하지 않는다.
    Core::MultiCallbackSystem<const char *> mCallbackSystem;

  protected:
    Logger();

  private:
};

} // namespace QuadLog
