#pragma once


#include<memory>
#include<string>
#include"Utility/Singleton.h"
#include"header.h"
#include"Core/CoreDllExport.h"


namespace Quad
{
	class RenderPassTwo;
	class GraphicPipelineStateGeneratorHelper;
	class Effect;

	class CORE_API_LIB CoreEffectInstaller:public Singleton<CoreEffectInstaller>
	{
	public:
		CoreEffectInstaller();
		~CoreEffectInstaller();

		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, 
			const std::string & effectFolderPath);
			
	//	Effect* GetRenderTargetToBackBufferEffect() const; 

	private:
		void InstallTestEffect();
		void SetTestEffectRenderPass0(RenderPassTwo * renderPass);
		void SetTestEffectRenderPass1(RenderPassTwo* renderPass);


		void LoadCompiledHLSL(const std::string& vsFilePath, const std::string& psFilePath, 
			GraphicPipelineStateGeneratorHelper* graphicPipelinStateGeneratorHelper,std::vector<std::unique_ptr<char[]>> & oRawBufferVector);


		void InstallRenderTargetToBackBufferEffect();


		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		std::string mEffectFolderPath;







		//mapLayer들의 디폴트렌더타켓을 최종후면버퍼로 렌더링하는 effect, 모든 렌더시스템에대해서 공통
		//또한 root constant사용으로 프로그램전체에대해 하나만존재하면됨.
		Effect* mRenderTargetToBackBuffereffect;


	};

}
