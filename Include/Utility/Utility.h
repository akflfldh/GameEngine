#pragma once
#include"header.h"
#include<locale>
#include<codecvt>



#ifdef UTILITY_LIB_EXPORT
#define UTILITY_LIB_API __declspec(dllexport)
#else
#define UTILITY_LIB_API __declspec(dllimport)
#endif



namespace Quad
{
	class UTILITY_LIB_API Utility
	{
	public:
		Utility() = delete;
		~Utility() = delete;


		static void* AllocAligned(size_t size, uint8_t align);
		static void FreeAligned(void* ptr);

		static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, void* pElements, UINT64 elementSize, UINT64 elementNum,
			Microsoft::WRL::ComPtr<ID3D12Resource>& mUploadBuffer);
		static size_t CalcConstantBufferByteSize(size_t size)
		{
			return (size + 255) & ~255;
		}


		static DirectX::XMFLOAT4X4 GetIdentityMatrixF();
		static DirectX::XMMATRIX GetIdentityMatrix();


		static std::string ConvertToString(const std::wstring & Str, bool unicode);
		static std::wstring ConvertToWString(const std::string& str, bool unicode);

		static std::vector<std::wstring> GetTokenSubString(const std::wstring& str, const std::vector<std::wstring>& delimiterVector);
		
		static std::string GetExtension(const std::string& filePath);		//잘못되면 return "";
		static std::wstring GetExtension(const std::wstring& filePath);		//잘못되면 return "";
		static std::string GetFileNameFromPath(const std::string& filePath);
		static std::wstring GetFileNameFromPath(const std::wstring& filePath);

		static std::string RemoveExtension(const std::string& fileName);

		static size_t CaculateConstantBufferElementOffset(int offset, int elementNum,int elementSize);


		static bool ConvertBoolValue(std::string str);


		static std::string GetFolderPathFromPickingDialog();
		

		static bool CheckFolderPath(const std::string& path);


		//기존의 디렉터리를 돌려준다.
		static std::string SetNewCurrentDirectory(const std::string& newDirectory);





		//void CaculateSkinningMeshNormalVector(std::vector<SkinningVertex>& vertexVector, const std::vector<MeshIndexType>& indexVector)
	private:



	};

}