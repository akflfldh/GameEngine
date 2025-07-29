#include "D3DRenderType.h"

D3D12_VIEWPORT D3DRender::ConvertToD3DViewport(const Render::Viewport & viewport) 
{

    D3D12_VIEWPORT d3dViewport;
    d3dViewport.Width =viewport.Width;
    d3dViewport.Height =viewport.Height;
    d3dViewport.TopLeftX =viewport.TopLeftX;
    d3dViewport.TopLeftY =viewport.TopLeftY;
    d3dViewport.MaxDepth =viewport.MaxDepth;
    d3dViewport.MinDepth =viewport.MinDepth;




    return d3dViewport;
}
