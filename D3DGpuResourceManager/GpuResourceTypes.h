#pragma once

enum class EResourceState
{
    eNone = 0,
    eRenderTarget,
    eGenericRead,
    ePresent,
    eCopySource,
    eCopyDest,
    eWriteDepthStencil,
    eReadDepthStencil
};