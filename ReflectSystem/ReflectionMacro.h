#pragma once

#include <ReflectSystem/ReflectionSystem.h>
#ifdef __clang__
#define REFLECT_CLASS(...) annotate("REFLECT_CLASS, " #__VA_ARGS__)
#define REFLECT_STRUCT(...) annotate("REFLECT_CLASS, " #__VA_ARGS__)
#define REFLECT_PROPERTY(...) annotate("REFLECT_PROPERTY, " #__VA_ARGS__)
#define annotate(text) __attribute__((annotate(text)))
#else
#define REFLECT_CLASS(...)
#define REFLECT_STRUCT(...)
#define REFLECT_PROPERTY(...)
#endif
#define GENERATED_BODY(ClassType)