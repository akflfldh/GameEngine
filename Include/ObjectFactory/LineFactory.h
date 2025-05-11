#pragma once


#include"Predeclare.h"
#include"header.h"
#include"Utility/Singleton.h"
#include"Object/Line/Line.h"


#include"Core/CoreDllExport.h"

namespace Quad
{

	class CORE_API_LIB LineFactory:public Singleton<LineFactory>
	{
	public:

		LineFactory();
		~LineFactory() = default;

		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device);

		//라인도 타입이있을수있다 . 직선,곡선,격자 ,
		static Line* CreateLine(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end);
		static Grid* CreateGrid(int rowNum, int columnNum, float cellWidth, float cellHeight);

		static Spline* CreateSpline(UINT vertexMaxNum);

		static bool ChangeGrid(Grid * grid, int rowNum, int columnNum, float cellWidth, float cellHeight);



		static void ReleaseLine(Line* line);
		static void ReleaseGrid(Grid* grid);
		static void ReleaseSpline(Spline* spline);
	

		static void ChangeVertexBuffer(LineBase* lineBase, UINT lineNum);




	private:
		static void MakeGridVertices(Grid* grid, int rowNum, int columnNum, float cellWidth, float cellHeight);


		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Line* mDefaultLine;


	};

}
