#pragma once


namespace Quad
{ 
	namespace EffectKeyDef
	{
		// pass에서의 사용되는 key 
		enum class EPassKey
		{
			eType=0,
			eShader,
			eConfig
		};

		//pass 안 shader의 key
		enum class EShaderKey
		{
			eFile= 0,
			eEntryPoint,
			eVersion,
		};

		
		enum class EShaderNameKey
		{
			eVS=0,
			ePS,
			eGS

		};


		 const char * toString(EPassKey passKey);
		 const char* toString(EShaderKey shaderKey);
		 const char* toString(EShaderNameKey shaderNameKey);

	}
		
}


