#pragma once

#undef GENERATED_BODY
#define GENERATED_BODY(ClassType) \
    friend class ClassInfo; \
    friend struct UIImageComponentRegistrar; \
	public :									\
		static constexpr const char * GetStaticClassName() { return #ClassType; } \
		virtual const char * GetTypeName(){return #ClassType;}				\
    // 여기에 메타데이터, 함수 포인터 선언 등 추가 리플렉션 코드를 넣을 수 있습니다. \

