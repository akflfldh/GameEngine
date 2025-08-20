#include "CoreDevice/CoreDevice.h"

#ifdef D3DX
#include "CoreDevice/D3DCoreDevice.h"

#endif

Core::CoreDevice *Core::CoreDevice::GetInstance()
{
#ifdef D3DX
    static D3DCoreDevice instance;

    return &instance;
#endif
}

Core::CoreDevice::CoreDevice() {}

Core::CoreDevice::~CoreDevice() {}
