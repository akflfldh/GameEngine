#pragma once

#include <ReflectSystem/ReflectionSystem.h>
#ifdef __clang__
#define REFLECT_CLASS annotate("REFLECT_CLASS")
#define REFLECT_PROPERTY annotate("REFLECT_PROPERTY")
#define annotate(text) __attribute__((annotate(text)))
#else
#define REFLECT_CLASS
#define REFLECT_PROPERTY
#endif
#define GENERATED_BODY(ClassType)