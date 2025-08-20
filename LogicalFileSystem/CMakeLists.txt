add_library(LogicalFileSystem STATIC

LogicalCommon.h
LogicalFile.cpp
LogicalFile.h
LogicalFileSystem.cpp
LogicalFileSystem.h
LogicalFolder.cpp
LogicalFolder.h
LogicalNode.cpp
LogicalNode.h
)

get_filename_component(PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)

target_include_directories(LogicalFileSystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PARENT_DIR})


target_link_libraries(LogicalFileSystem PUBLIC compiler_flags  headerDir BinaryReaderWriter PhysicalFileSystem CoreAsset)


