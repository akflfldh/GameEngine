#pragma once


#include"Predeclare.h"
#include"Object/Line/LineBase.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB Grid:public LineBase
	{
		friend class LineFactory;

	public:
		~Grid() = default;



		static Grid* Create(Map* map, int mapLayerIndex, int rowNum, int columnNum, float cellWidth, float cellHeight);



		virtual void Initialize() override;
		virtual void DefaultCreatingInitialize() override;
		virtual void OnEvent(Event* event) override;
		virtual void Update(float deltaTime) override;
		virtual void EndUpdate(float deltaTime) override;
		//virtual void UpdateCollider(float deltaTime) override;
	
		CLASSNAME(Grid)
		CLASSSIZE(Grid)
		virtual void Serialize()override;
		virtual void DeSerialize()override;
	protected:
		void InitCreating(int rowNum, int columnNum, float cellWidth, float cellHeight);

	private:


		Grid(const std::string& name = "");
		int mRowNum;
		int mColumnNum;


	};
}

