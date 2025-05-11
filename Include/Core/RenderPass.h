#pragma once

#include"Predeclare.h"
#include<vector>
#include"header.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB RenderPass
	{
		friend class ResourceController;

	public:
		RenderPass() = default;
		~RenderPass() = default;


		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState() const;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const;
		const std::vector<std::pair<ShaderResource*, int>> &  GetShaderResourcePerPassVector() const;
		const std::vector<std::pair<ShaderResource*, int>> &  GetShaderResourcePerObjectVector() const;
		//const std::vector<std::pair<std::wstring, int>>& GetShaderResourcePerPassVector() const;
		//const std::vector<std::pair<std::wstring, int>>& GetShaderResourcePerObjectVector() const;
	private:
		void SetPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState);
		void SetRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;

		//pass별 리소스와
		//object별 리소스로나누어서 관리한다.
		//그러면 몇번 루트 매개변수와 대응되는지를 나타내야한다.
							//shader resource , root paraemter index
		std::vector<std::pair<ShaderResource*,int>> mShaderResourcePerPassVector;
		std::vector<std::pair<ShaderResource*,int>> mShaderResourcePerObjectVector;//더정확히는 subObject별

		//std::vector<std::pair<std::wstring, int>> mShaderResourcePerPassVector;
		//std::vector<std::pair<std::wstring, int>> mShaderResourcePerObjectVector;//더정확히는 subObject별
	};

}
