add_library(InputSystem SHARED 
InputSystem.cpp 
InputSystem.h 
InputSystemDllMacro.h
InputType.h 
 "D3DInputSystem.h" "D3DInputSystem.cpp" "IInputHandler.h" "IInputHandler.cpp")


target_compile_definitions(InputSystem PRIVATE INPUT_SYSTEM_LIB)

target_include_directories(InputSystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(InputSystem PUBLIC compiler_flags headerDir CoreMath)


