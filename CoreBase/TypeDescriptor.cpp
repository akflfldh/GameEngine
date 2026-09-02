#include "TypeDescriptor.h"
#include <CoreBase/Arch.h>
#include <cstring>
#include <stdint.h>

TypeDescriptor::TypeDescriptor() {}

TypeDescriptor::~TypeDescriptor() {}

char TypeDescriptor::GetTypeSize() const
{
    return mTypeSize;
}

void TypeDescriptor::SetTypeSize(char size)
{
    mTypeSize = size;
}

// ========== TypeDescriptor_char ==========
void TypeDescriptor_char::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    char *pData = static_cast<char *>(data);
    arch << *pData;
}

void TypeDescriptor_char::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(char));
}

TypeDescriptor_char::TypeDescriptor_char()
{
    SetTypeSize(sizeof(char));
}

// ========== TypeDescriptor_unsigned_char ==========
void TypeDescriptor_unsigned_char::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    unsigned char *pData = static_cast<unsigned char *>(data);
    arch << *pData;
}

void TypeDescriptor_unsigned_char::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(unsigned char));
}

TypeDescriptor_unsigned_char::TypeDescriptor_unsigned_char()
{
    SetTypeSize(sizeof(unsigned char));
}

// ========== TypeDescriptor_short ==========
void TypeDescriptor_short::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    short *pData = static_cast<short *>(data);
    arch << *pData;
}

void TypeDescriptor_short::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(short));
}

TypeDescriptor_short::TypeDescriptor_short()
{
    SetTypeSize(sizeof(short));
}

// ========== TypeDescriptor_unsigned_short ==========
void TypeDescriptor_unsigned_short::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    unsigned short *pData = static_cast<unsigned short *>(data);
    arch << *pData;
}

void TypeDescriptor_unsigned_short::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(unsigned short));
}

TypeDescriptor_unsigned_short::TypeDescriptor_unsigned_short()
{
    SetTypeSize(sizeof(unsigned short));
}

// ========== TypeDescriptor_int ==========
void TypeDescriptor_int::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    int *pData = static_cast<int *>(data);
    arch << *pData;
}

void TypeDescriptor_int::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(int));
}

TypeDescriptor_int::TypeDescriptor_int()
{
    SetTypeSize(sizeof(int));
}

// ========== TypeDescriptor_unsigned_int ==========
void TypeDescriptor_unsigned_int::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    unsigned int *pData = static_cast<unsigned int *>(data);
    arch << *pData;
}

void TypeDescriptor_unsigned_int::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(unsigned int));
}

TypeDescriptor_unsigned_int::TypeDescriptor_unsigned_int()
{
    SetTypeSize(sizeof(unsigned int));
}

// ========== TypeDescriptor_long ==========
void TypeDescriptor_long::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    long *pData = static_cast<long *>(data);
    arch << *pData;
}

void TypeDescriptor_long::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(long));
}

TypeDescriptor_long::TypeDescriptor_long()
{
    SetTypeSize(sizeof(long));
}

// ========== TypeDescriptor_unsigned_long ==========
void TypeDescriptor_unsigned_long::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    unsigned long *pData = static_cast<unsigned long *>(data);
    arch << *pData;
}

void TypeDescriptor_unsigned_long::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(unsigned long));
}

TypeDescriptor_unsigned_long::TypeDescriptor_unsigned_long()
{
    SetTypeSize(sizeof(unsigned long));
}

// ========== TypeDescriptor_long_long ==========
void TypeDescriptor_long_long::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    long long *pData = static_cast<long long *>(data);
    arch << *pData;
}

void TypeDescriptor_long_long::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(long long));
}

TypeDescriptor_long_long::TypeDescriptor_long_long()
{
    SetTypeSize(sizeof(long long));
}

// ========== TypeDescriptor_unsigned_long_long ==========
void TypeDescriptor_unsigned_long_long::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    unsigned long long *pData = static_cast<unsigned long long *>(data);
    arch << *pData;
}

void TypeDescriptor_unsigned_long_long::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(unsigned long long));
}

TypeDescriptor_unsigned_long_long::TypeDescriptor_unsigned_long_long()
{
    SetTypeSize(sizeof(unsigned long long));
}

// ========== TypeDescriptor_size_t ==========
void TypeDescriptor_size_t::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    size_t *pData = static_cast<size_t *>(data);
    arch << *pData;
}

void TypeDescriptor_size_t::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(size_t));
}

TypeDescriptor_size_t::TypeDescriptor_size_t()
{
    SetTypeSize(sizeof(size_t));
}

// ========== TypeDescriptor_float ==========
void TypeDescriptor_float::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    float *pData = static_cast<float *>(data);
    arch << *pData;
}

void TypeDescriptor_float::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(float));
}

TypeDescriptor_float::TypeDescriptor_float()
{
    SetTypeSize(sizeof(float));
}

// ========== TypeDescriptor_double ==========
void TypeDescriptor_double::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    double *pData = static_cast<double *>(data);
    arch << *pData;
}

void TypeDescriptor_double::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(double));
}

TypeDescriptor_double::TypeDescriptor_double()
{
    SetTypeSize(sizeof(double));
}

// ========== TypeDescriptor_bool ==========
void TypeDescriptor_bool::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{
    bool *pData = static_cast<bool *>(data);
    arch << *pData;
}

void TypeDescriptor_bool::Copy(void *source, void *dest)
{
    memcpy(dest, source, sizeof(bool));
}

TypeDescriptor_bool::TypeDescriptor_bool()
{
    SetTypeSize(sizeof(bool));
}
