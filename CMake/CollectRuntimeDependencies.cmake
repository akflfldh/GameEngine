cmake_minimum_required(VERSION 3.21)


if(NOT DEFINED GAME_OUTPUT_DIR)
    message(FATAL_ERROR "GAME_OUTPUT_DIR is not defined")
endif()

if(NOT DEFINED RUNTIME_SEARCH_DIR)
    message(FATAL_ERROR "RUNTIME_SEARCH_DIR is not defined")
endif()



set(gameDirectorPath "${GAME_OUTPUT_DIR}/GameDirector.exe")


file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES "${gameDirectorPath}"
    DIRECTORIES 
        "${GAME_OUTPUT_DIR}"
        "${RUNTIME_SEARCH_DIR}"


    PRE_EXCLUDE_REGEXES
        "^api-ms-.*"
        "^ext-ms-.*"

    
    POST_EXCLUDE_REGEXES
        ".*[Ww][Ii][Nn][Dd][Oo][Ww][Ss][/\\\\][Ss]ystem32[/\\\\].*"
     
        
    RESOLVED_DEPENDENCIES_VAR resolvedDlls
    UNRESOLVED_DEPENDENCIES_VAR unresolvedDlls
    CONFLICTING_DEPENDENCIES_PREFIX runtimeConflict
)

if(runtimeConflict_FILENAMES)
    message(FATAL_ERROR
            "Conflicting runtime dependencies: ${runtimeConflict_FILENAMES}")
endif()


if(unresolvedDlls)
    message(FATAL_ERROR
            "Unresolved runtime dependencies: ${unresolvedDlls}")
endif()



foreach(runtimeDll IN LISTS resolvedDlls)
    get_filename_component(runtimeDllName "${runtimeDll}" NAME)
    

    file(COPY_FILE 
         "${runtimeDll}"
         "${GAME_OUTPUT_DIR}/${runtimeDllName}"
          ONLY_IF_DIFFERENT)


    message(STATUS "Copied runtime DLL: ${runtimeDllName}")
endforeach()


