#pragma once


#include"Logger.h"
#include<fstream>
#include<string>
#include<unordered_map>
#include<mutex>


namespace QuadLog
{

	class LoggerImpl :public Logger
	{
	public:
		LoggerImpl();
		virtual ~LoggerImpl();
		//Initialize이다.
		virtual bool SetLoggerFile(const char* outputFile) override;



		virtual void Log(ELogLevel logLevel, const char* category, const char* message) override;
		virtual void Log(const char* logLevel, const char* category, const char* message) override;
		virtual void LogInfo(const char* category, const char* message) override;
		virtual void LogWarning(const char* category, const char* message) override;
		virtual void LogError(const char* category, const char* message) override;
		virtual void LogCritical(const char* category, const char* message) override;
		virtual void OnActive(int filterID) override;
		virtual void OffActive(int filterID) override;


		virtual void SetMinLogLevel(ELogLevel minLogLevel) override;




	private:

		std::string BuildLogMessage(ELogLevel logLevel, const char* category, const char* message);
		ELogLevel ParseLogLevel(const char* logLevelStr) const;

	private:
		std::ofstream fout;
		std::string mOutputFile;
		std::mutex mLogMutex;

		std::unordered_map<ELogLevel, std::string> mLogLevelStringTable;
		std::unordered_map<std::string, ELogLevel> mLogLevelEnumTable;

		ELogLevel mMinLogLevel;

		bool mFileActiveFlag;
		int mNextID;


	};


}