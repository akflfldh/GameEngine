#pragma once

#include"Predeclare.h"
#include"header.h"
#include"CoreDllExport.h"
namespace Quad
{
	enum class EBoneType
	{
		eBone=0,
		eMesh,
		eNull
	};

	class CORE_API_LIB Bone
	{
	

	public:
		Bone();
		~Bone() = default;

		void SetName(const std::string& name);
		const std::string& GetName() const;

		void SetBoneType(Quad::EBoneType type);

		EBoneType GetBoneType() const;


		void Serialize(const std::string& tag);
		void DeSerialize(const std::string& tag);

	private:
		std::string mName;
		EBoneType mBoneType;
		DirectX::XMFLOAT3 mPosition;
		DirectX::XMFLOAT3 mScale;
		DirectX::XMFLOAT3 mRotation;

	};

}