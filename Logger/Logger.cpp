#include "Logger.h"
#include"LoggerImpl.h"
QuadLog::Logger* QuadLog::Logger::GetInstance()
{
	static QuadLog::LoggerImpl instance;
	return &instance;
}


QuadLog::Logger::Logger()
{

}

QuadLog::Logger::~Logger()
{


}
