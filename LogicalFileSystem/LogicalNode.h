#pragma once


#include<string>
#include"LogicalCommon.h"
namespace QuadLF
{


	class LogicalNode
	{
		friend class LogicalFileSystem;
	public:
		LogicalNode(LogicalNodeID id, ELogicalNodeType nodeType);
		virtual ~LogicalNode() = 0;

		const std::string & GetName() const;
		LogicalNodeID GetNodeID()const;
		ELogicalNodeType GetNodeType() const;


		std::string GetFullPath() const;




		LogicalNode* GetParent() const;
		//내부적으로 파일시스템의 메서드를 호출한다.
		void SetParent(LogicalNode* parent);



		bool GetPhyscicalBindingFlag() const;
	protected:
		void SetName(const std::string& name);
		void SetPhysicalBindingFlag(bool flag);

	private:
	
		



	private:
		LogicalNodeID mID;
		std::string mName;			//display name (folder name or file name) 


		LogicalNode* mParent;
		ELogicalNodeType mNodeType;

		//물리적 파일,폴더와 연결된 노드여부 플래그
		bool mIsPhysicalNodeBinding;

	};

}
