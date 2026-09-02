// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include <Windows.h>
#include <string>

#define IMPLEMENT_GAME_MODULE(PROJECTNAME)                                                                             \
                                                                                                                       \
    BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)                                \
    {                                                                                                                  \
        switch (ul_reason_for_call)                                                                                    \
        {                                                                                                              \
        case DLL_PROCESS_ATTACH:                                                                                       \
        case DLL_THREAD_ATTACH:                                                                                        \
        case DLL_THREAD_DETACH:                                                                                        \
        case DLL_PROCESS_DETACH:                                                                                       \
            break;                                                                                                     \
        }                                                                                                              \
        return TRUE;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    extern "C" __declspec(dllexport) void InitializeUserModule()                                                       \
    {                                                                                                                  \
        std::string str = "[" #PROJECTNAME "] 게임 모듈 초기화 함수 실행됨! (에디터가 날 불렀다!)";                    \
                                                                                                                       \
        OutputDebugStringA(str.c_str());                                                                               \
    }