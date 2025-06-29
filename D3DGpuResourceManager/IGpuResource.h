#pragma once


namespace GRM
{
	enum class EGpuResourceType
	{
		eBuffer = 0,
		e2DTexture,
		eSampler,
	};

	class IGpuResource
	{
	public:
		virtual ~IGpuResource();

		GRM::EGpuResourceType GetGpuResourceType() const;

	protected:
		IGpuResource();

		GRM::EGpuResourceType mGpuResourceType;
	};


}