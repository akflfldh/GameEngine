add_library(Logger SHARED
Logger.h
Logger.cpp
LoggerImpl.cpp
LoggerImpl.h
)
# 이 타깃을 컴파일할 때만 export 모드가 되도록
target_compile_definitions(Logger PRIVATE QUAD_LOG_EXPORT)


target_include_directories(Logger PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# MultiCallbackSystem은 헤더 전용이다. CoreBase를 링크하면 PhysicalFileSystem을 거쳐 Logger로 순환한다.
# 기존 headerDir가 제공하는 소스 루트 경로로 공통 콜백 헤더를 사용한다.
target_link_libraries(Logger PUBLIC compiler_flags headerDir)
