#pragma once
#include <CoreBase/Arch.h>
#include <CoreBase/CoreBaseDllMacro.h>
#include <vector>
class Arch;

// 타입디스크립터를 어느 모듈에 넣어야할지 고려해야할것, reflectionosysyem인지 corebase인지

namespace Quad
{
struct PropertyInfo;
}

class COREBASE_API TypeDescriptor
{
  public:
    TypeDescriptor();
    virtual ~TypeDescriptor();

    virtual void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr) = 0;

    char GetTypeSize() const;

  protected:
    void SetTypeSize(char size);

  private:
    char mTypeSize = 0;
};

class COREBASE_API ITypeDescriptor_BuiltIn
{
  public:
    virtual ~ITypeDescriptor_BuiltIn() = default;
    virtual void Copy(void *source, void *dest) = 0;
};

class COREBASE_API TypeDescriptor_char : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_char();

    void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo) override;
    virtual void Copy(void *source, void *dest) override;
};

class COREBASE_API TypeDescriptor_unsigned_char : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_unsigned_char();

    void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo) override;
    virtual void Copy(void *source, void *dest) override;
};

class COREBASE_API TypeDescriptor_short : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_short();

    void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr) override;
    virtual void Copy(void *source, void *dest) override;
};

class COREBASE_API TypeDescriptor_unsigned_short : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_unsigned_short();

    virtual void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr) override;
    virtual void Copy(void *source, void *dest) override;
};

class COREBASE_API TypeDescriptor_int : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_int();

    virtual void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr) override;
    virtual void Copy(void *source, void *dest) override;
};

class COREBASE_API TypeDescriptor_unsigned_int : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_unsigned_int();

    virtual void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr) override;
    virtual void Copy(void *source, void *dest) override;
};

class COREBASE_API TypeDescriptor_long : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_long();

    virtual void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr) override;
    virtual void Copy(void *source, void *dest) override;
};

class COREBASE_API TypeDescriptor_unsigned_long : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_unsigned_long();

    virtual void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr) override;
    virtual void Copy(void *source, void *dest) override;
};

class COREBASE_API TypeDescriptor_long_long : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_long_long();

    virtual void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr) override;
    virtual void Copy(void *source, void *dest) override;
};

class COREBASE_API TypeDescriptor_unsigned_long_long : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_unsigned_long_long();

    virtual void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr) override;
    virtual void Copy(void *source, void *dest) override;
};

class COREBASE_API TypeDescriptor_float : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_float();

    virtual void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr) override;
    virtual void Copy(void *source, void *dest) override;
};

class COREBASE_API TypeDescriptor_double : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_double();

    virtual void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr) override;
    virtual void Copy(void *source, void *dest) override;
};

class COREBASE_API TypeDescriptor_size_t : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_size_t();

    virtual void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr) override;
    virtual void Copy(void *source, void *dest) override;
};

class COREBASE_API TypeDescriptor_bool : public TypeDescriptor, public ITypeDescriptor_BuiltIn
{
  public:
    TypeDescriptor_bool();

    virtual void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr) override;
    virtual void Copy(void *source, void *dest) override;
};