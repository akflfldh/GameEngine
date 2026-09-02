cmake_minimum_required(VERSION 3.20)
project(ClangCodeGenerator CXX)

# C++ 표준 버전만 설정합니다. (17 또는 20 모두 가능)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# vcpkg toolchain 파일이 모든 것을 처리하므로, find_package만 호출하면 됩니다.
# vcpkg가 18.1.6 버전을 설치했으므로, 버전을 정확히 명시해줍니다.
find_package(LLVM 18.1.6 EXACT REQUIRED CONFIG)
find_package(Clang 18.1.6 EXACT REQUIRED CONFIG)

add_executable(CodeGenerator src/main.cpp
src/IntermediateGenerator.cpp
src/IntermediateGenerator.h
src/IntermediateFrontendAction.cpp
src/IntermediateFrontendAction.h
src/ReflectType.h
 "src/IntermediateASTConsumer.h" "src/IntermediateASTConsumer.cpp" "src/IntermediateASTVisitor.cpp" "src/IntermediateASTVisitor.h"   "src/DepFileCollector.h" "src/DepFileCollector.cpp" "src/IntermediateFrontendFactory.h" "src/IntermediateFrontendFactory.cpp")

# 필요한 최소한의 라이브러리만 나열합니다.
# vcpkg가 나머지 의존성은 자동으로 처리해줍니다.
target_link_libraries(CodeGenerator PRIVATE
    clangTooling
    clangFrontend
    clangAST
    clangBasic
    LLVMSupport
)