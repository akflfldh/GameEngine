

#include <fstream>
#include <iostream>
#include <string>
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "인자를 정확히 넘겨야합니다.\n";
        return 0;
    }

    std::string generatedFileName = std::string(argv[1]) + ".generated.h";
    std::ofstream fout(generatedFileName);

    fout << "#pragma once\n\n";
    fout << "#include <ReflectSystem/ReflectionSystem.h>\n";
    // GENERATED_BODY() 매크로 정의를 .generated.h 파일에 작성합니다.
    // 여기에 friend 선언과 기타 필요한 리플렉션 코드를 추가합니다.
    // **** 이 부분을 수정하세요! ****

    fout << "#ifdef __clang__\n";
    // 생성기가 찾을 마커(Marker) 매크로입니다.
    // Clang의 annotate 속성을 사용하여 AST(추상 구문 트리)에 정보를 남깁니다.
    fout << "#define REFLECT_CLASS annotate(\"REFLECT_CLASS\")\n";
    fout << "#define REFLECT_PROPERTY annotate(\"REFLECT_PROPERTY\")\n";

    // __attribute__((annotate(text)))는 Clang 컴파일러에게
    // "이 부분에 'text'라는 이름표를 붙여줘" 라고 알려주는 특별한 기능입니다.
    fout << "#define annotate(text) __attribute__((annotate(text)))\n";
    fout << "#else\n";
    fout << "#define REFLECT_CLASS\n";
    fout << " #define REFLECT_PROPERTY\n";
    fout << "#endif\n";

    fout << "#define GENERATED_BODY(ClassType) \n";
}