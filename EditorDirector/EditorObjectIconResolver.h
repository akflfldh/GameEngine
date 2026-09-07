#pragma once

class Object;

class EditorObjectIconResolver
{
  public:
    static const char *Resolve(const Object &object);
};
