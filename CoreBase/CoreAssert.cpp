#include <CoreBase/CoreAssert.h>
#include <string>

#ifdef D3DX

#include <windows.h>

#endif

bool ReportAssertFailure(const char *condition, const char *file, int line, const char *msg, ...)
{
    using namespace std;

    char buffer[1024];
    va_list args; // va_list의 정체는 생각보다 아주 단순합니다. 한마디로 정의하자면 **"현재 읽어야 할 가변 인자의 메모리
                  // 위치를 가리키는 포인터(커서)"**입니다.
    va_start(args, msg);
    vsnprintf(buffer, sizeof(buffer), msg, args);
    va_end(args);

    std::string fullMessage = "Condition : " + string(condition) + "\n\n" + "Message : " + string(buffer) + "\n\n" +
                              "File : " + string(file) + "\n\n" + "Line : " + to_string(line) + "\n\n";

#ifdef D3DX

    MessageBoxA(nullptr, fullMessage.c_str(), "Assert", MB_OK);

#endif

    return true;
}

bool COREBASE_API ReportAssertFailure(const char *condition, const char *file, int line)
{

    return ReportAssertFailure(condition, file, line, "");
}
