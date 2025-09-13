add_library(Utility STATIC 
Utility.cpp
Utility.h
uuid.h
  "UniqueID.h" "UniqueID.cpp")


target_include_directories(Utility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


find_package(ICU REQUIRED COMPONENTS uc i18n data)
target_link_libraries(Utility PUBLIC compiler_flags headerDir PRIVATE ICU::uc ICU::i18n ICU::data)


target_precompile_headers(Utility
    PRIVATE
        pch.h
)