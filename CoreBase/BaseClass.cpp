#include "BaseClass.h"
BaseClass::BaseClass() {};
BaseClass::~BaseClass() {}

const char *BaseClass::GetRunTimeClassName() const
{
    return "BaseClass";
}
void BaseClass::MarkPropertyDirty()
{

    mPropertyDirty = true;
}

bool BaseClass::GetPropertyDirty() const
{

    return mPropertyDirty;
}

void BaseClass::ClearPropertyDirty()
{

    mPropertyDirty = false;
}

void BaseClass::FlushPropertyDirty() {}