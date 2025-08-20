add_library(BinaryReaderWriter STATIC
BinaryReader.h
BinaryWriter.h
BinaryReader.cpp
BinaryWriter.cpp
)


target_include_directories(BinaryReaderWriter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



target_link_libraries(BinaryReaderWriter PUBLIC compiler_flags headerDir PhysicalFileSystem)
