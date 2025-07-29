#pragma once

#include"LogicalNode.h"


namespace QuadLF
{
	class LogicalFile :public LogicalNode
	{
		friend class LogicalFileSystem;
	public:
		virtual ~LogicalFile();


		


		void SetAssetInfo(const LogicalFileAssetInfo& logicalFileAssetInfo);





		LogicalFileAssetInfo& GetAssetInfo();
		const LogicalFileAssetInfo& GetAssetInfo() const;
	private:
		LogicalFile(LogicalNodeID id);

	private:
		LogicalFileAssetInfo mAssetInfo;

		//asset의 메타데이터 구조체를 멤버변수로 가진다.



	};

}
