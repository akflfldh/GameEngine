add_library(JsonParser STATIC
IParserImpl.cpp
IParserImpl.h
Json_Parser_Wrapping.cpp
JsonParser.cpp
JsonParser.h
JsonParserType.cpp
JsonParserType.h
RapidJsonParserImpl.cpp
RapidJsonParserImpl.h
)


target_include_directories(JsonParser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(JsonParser PUBLIC compiler_flags headerDir)