# GameEngine



/* 실행을 위한 사전 작업

1. GitHub Release / EngineDependencies.v.0.0.1에서 EngineDependencies-v0.0.1.zip을 다운로드합니다. 
2.압축을 해제합니다.
3.다음과 같이 배치합니다.

GameEngine/
- ClangCodeGenerator.exe
- zstd.dll
- zlibd1.dll

GameEngine/QuadCallbackSystemDLL/
-QuadCallbackSystem.lib
-QuadCallbackSystem.dll


4.vcpkg 최상위 폴더경로를 환경변수 VCPKG_ROOT로 설정해야합니다.

5.CMakeUserpresets.json파일 생성후 캐시변수 FBXSDK_ROOT를 fbxsdk include와 ,lib가 존재하는 폴더경로로 설정해야합니다.
(CMakeUserpresets.json.example예시 파일이 존재)




ProjectSelector가 참조하는 ProjectMetaDataFile.json의 Test프로젝트의 경로는 절대경로이기에 ProjectSelector실행시 이 파일을 수정해야합니다.

*/