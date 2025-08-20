# GameEngine



/* 실행을 위한 사전 작업


git Release에서 CodeGenerator.exe, zstd.dll, zlibd1.dll을 다운받아. GameEngine폴더에 넣어야합니다.
(zstd.dll, zlibd1.dll은 clangCodeGenerator.exe실행을 위해 필요한 dll입니다.)


CMakePresets.json 내에서
cacheVariables":
    "CAMKE_TOOLCHAIN_FILE" 을 해당 자신의 vcpkg.cmake가 위치한 경로로 수정해야합니다.
    

ProjectSelector가 참조하는 ProjectMetaDataFile.json의 Test프로젝트의 경로는 절대경로이기에 ProjectSelector실행시 이 파일을 수정해야합니다.

*/