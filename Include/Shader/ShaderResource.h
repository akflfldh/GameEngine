#pragma once

#include"Predeclare.h"
#include<string>

#include"DescriptorHeapType.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	enum class  EShaderResourceType
	{
		eConstantBuffer=0,
		eTexture,
		eSampler
	};

	enum class  EShaderResourceVisibility
	{
		eAll=0,
		eVertex,
		eHull,
		eDomain,
		eGeometry,
		ePixel,
		eAmplification,
		eMesh
	};




	class CORE_API_LIB ShaderResource
	{
	public:
		ShaderResource(EShaderResourceType type,const std::wstring & name , char registerIndex, char registerSpace);
		virtual ~ShaderResource() = 0;

	
		const std::wstring& GetName() const;
		EShaderResourceType GetShaderResourceType() const;
		
		char GetRegisterSpace() const;
		char GetRegisterIndex() const;

		void SetShaderResourceVisibility(EShaderResourceVisibility shaderResourceVisibility);
		EShaderResourceVisibility GetShaderResourceVisibility()const;
		void SetObjectOrPassFlag(bool flag);
		bool GetObjectOrPassFlag()const;

	private:
		EShaderResourceType mType;
		std::wstring mName;		//ID
		

		char mRegisterSpace;
		char mRegisterIndex;

		EShaderResourceVisibility mShaderResourceVisibility;
		
		bool mObjectOrPassFlag = true;       //true : object, falst: pass
	};


}
