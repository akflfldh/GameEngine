#pragma once
#include "Shader/ShaderResource.h"
#include"DescriptorHeapType.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
    class  CORE_API_LIB ShaderResourceSampler :
        public ShaderResource
    {

    public:
        ShaderResourceSampler(const std::wstring& name, char registerIndex, char registerSpace,ViewIndex viewIndex);
        ~ShaderResourceSampler() = default;

        ViewIndex GetViewIndex()const;
       


    private:
        //Sampler resource pointer
        ViewIndex mViewIndex;

    };

}
