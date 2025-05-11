#include "ObjectFactory/LineFactory.h"
#include"header.h"
#include"Object/Line/Line.h"
#include"Object/Line/Grid.h"
#include"Buffer/UploadBuffer.h"


#include"Object/RegisterObjectHelper.h"

#include"Core/ObjectBinnedAllocator.h"

#include"Object/Line/Spline.h"


Quad::LineFactory::LineFactory()
{
	RegisterObjectHelper lineCtorDtor("Line", []() ->Object* {
		Line* line = LineFactory::CreateLine({ 0,0,0 }, { 100.0f,100.0f,100.0f });
		return line; },  
		[](Object* ob) { LineFactory::ReleaseLine((Line*)ob);});
	

	RegisterObjectHelper gridCtorDtor("Grid", []() ->Object* {
		Grid* grid = LineFactory::CreateGrid(10,10,50,50);
		return grid; },
	[](Object* ob) { LineFactory::ReleaseGrid((Grid*)ob); });

	RegisterObjectHelper SplineCtorDtor("Spline", []() ->Object* {
		Spline* spline = LineFactory::CreateSpline(16);
		return spline; },
		[](Object* ob) { LineFactory::ReleaseSpline((Spline*)ob); });

}

void Quad::LineFactory::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device)
{
	mDevice = device;

	//std::vector<LineVertex> lineVertexVector(2);
	//lineVertexVector[0].mPos = { -50.0f,0.0,10.0f };
	//lineVertexVector[1].mPos = { 50.0f,0,10};


	//UploadBuffer* vertexUploadBuffer = new UploadBuffer(device, sizeof(LineVertex), 2, false);


	//vertexUploadBuffer->CopyData(0, &lineVertexVector[0], sizeof(LineVertex));
	//vertexUploadBuffer->CopyData(1, &lineVertexVector[1], sizeof(LineVertex));
//	mDefaultLine = new Line;
//	mDefaultLine->SetVertexBuffer(vertexUploadBuffer);


}

Quad::Line* Quad::LineFactory::CreateLine(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end)
{
	auto instance = GetInstance();
	ID3D12Device* device = instance->mDevice.Get();
	//Line* line = new Line("");
	Line* line = new (GetObjectBinnedAllocator(sizeof(Line))->Alloc())Line;
	// Line은 인덱스버퍼를 사용하지않아도될거같다.
	Line * defaultLine = instance->mDefaultLine;


	UploadBuffer* vertexBuffer = new UploadBuffer(device, sizeof(LineVertex), 2, false);
	

	vertexBuffer->CopyData(0, &start, sizeof(LineVertex));
	vertexBuffer->CopyData(1, &end, sizeof(LineVertex));

	line->SetVertexBuffer(vertexBuffer);
	line->SetVertexNum(2);


	return line;


}

Quad::Grid* Quad::LineFactory::CreateGrid(int rowNum, int columnNum, float cellWidth, float cellHeight)
{
	Grid* grid =new (GetObjectBinnedAllocator(sizeof(Grid))->Alloc()) Grid;

	static long long nameCount = 0;
	MakeGridVertices(grid, rowNum, columnNum, cellWidth, cellHeight);

	return grid;
}

Quad::Spline* Quad::LineFactory::CreateSpline(UINT vertexMaxNum)
{
	auto instance = GetInstance();
	ID3D12Device* device = instance->mDevice.Get();
	//Line* line = new Line("");
	Spline* spline = new (GetObjectBinnedAllocator(sizeof(Spline))->Alloc())Spline;
	// Line은 인덱스버퍼를 사용하지않아도될거같다.
	UploadBuffer* vertexBuffer = new UploadBuffer(device, sizeof(LineVertex), vertexMaxNum, false);

	spline->SetVertexBuffer(vertexBuffer);
	spline->SetVertexNum(vertexMaxNum);


	return spline;
}

bool Quad::LineFactory::ChangeGrid(Grid* grid, int rowNum, int columnNum, float cellWidth, float cellHeight)
{
	if (grid == nullptr)
		return false;


	//기존 grid의 vertex UploadBuffer는 제거한다.
	delete grid->mUploadVertexBuffer;

	MakeGridVertices(grid, rowNum, columnNum, cellWidth, cellHeight);

	return true;
}

void Quad::LineFactory::ReleaseLine(Line* line)
{

	delete line->GetVertexBuffer();

	line->~Line();


	GetObjectBinnedAllocator(sizeof(Line))->Release(line);


	

}

void Quad::LineFactory::ReleaseGrid(Grid* grid)
{
	delete grid->GetVertexBuffer();

	grid->~Grid();

	GetObjectBinnedAllocator(sizeof(Grid))->Release(grid);


}

void Quad::LineFactory::ReleaseSpline(Spline* spline)
{
	delete spline->GetVertexBuffer();

	spline->~Spline();








	GetObjectBinnedAllocator(sizeof(Spline))->Release(spline);








}

void Quad::LineFactory::ChangeVertexBuffer(LineBase* lineBase, UINT lineNum)
{

	auto instnace = GetInstance();
	delete lineBase->mUploadVertexBuffer;

	lineBase->mUploadVertexBuffer = new UploadBuffer(instnace->mDevice, sizeof(DirectX::XMFLOAT3), lineNum, false);
	lineBase->mVertexNum = lineNum;

}

void Quad::LineFactory::MakeGridVertices(Grid* grid, int rowNum, int columnNum, float cellWidth, float cellHeight)
{


	auto instance = GetInstance();
	ID3D12Device* device = instance->mDevice.Get();


	std::vector<LineVertex> lineVertexVector((rowNum) * 2 + (columnNum * 2));
	//std::vector<LineVertex> lineVertexVector((rowNum) * 2);
	//std::vector<LineVertex> lineVertexVector(4);


	int centerColumnIndex = (columnNum - 1) / 2;
	int centerRowIndex = (rowNum - 1) / 2;
	int leftColumnNum = centerColumnIndex; //중앙 column을기준으로 왼쪽에있는 column의개수 ,(중앙column은 제외)
	int rightColumnNum = centerColumnIndex;	// 오른쪽에있는 column의 개수(중앙 column은제외)
	int topRowNum = centerRowIndex;	//동일
	int bottomRowNum = centerRowIndex;

	if (columnNum % 2 == 0)
	{//짝수라면 왼쪽은 한개 적다
		leftColumnNum -= 1;
	}
	if (rowNum % 2 == 0)
	{
		topRowNum -= 1;
	}

	float startPosX = -cellWidth * leftColumnNum;
	float startPosZ = -cellHeight * topRowNum;
	float endPosX = cellWidth * rightColumnNum;
	float endPosZ = cellHeight * bottomRowNum;

	//lineVertexVector[0].mPos = { -1.0f,2.0,10.0f };
	//lineVertexVector[1].mPos = { 1.0f,2,10 };

	//lineVertexVector[2].mPos = { -25.0f,8.0,50.0f };
	//lineVertexVector[3].mPos = { 25.0f,8,-30 };
	//lineVertexVector[0].mPos = { startPosX + 500 * cellWidth,0.0f,startPosZ };//start
	//lineVertexVector[1].mPos = { startPosX + 500 * cellWidth,0.0f,endPosZ };//end
	int lineVertexIndex = 0;
	float y = -10.0f;
	for (int i = 0; i < columnNum; ++i)
	{
		lineVertexVector[lineVertexIndex++].mPos = { startPosX + i * cellWidth,y,startPosZ };//start
		lineVertexVector[lineVertexIndex++].mPos = { startPosX + i * cellWidth,y,endPosZ };//end
	}


	for (int i = 0; i < rowNum; ++i)
	{
		lineVertexVector[lineVertexIndex++].mPos = { startPosX,y ,startPosZ + i * cellHeight };//start
		lineVertexVector[lineVertexIndex++].mPos = { endPosX,y,startPosZ + i * cellHeight };//end
	}

	//	Grid* grid = new Grid;
//	Grid* grid = static_cast<Grid*>(GetObjectBinnedAllocator(sizeof(Grid))->Alloc());

	UploadBuffer* uploadBuffer = new UploadBuffer(device, sizeof(LineVertex), lineVertexVector.size(), false);
	//static long long nameCount = 0;
	//grid->SetName("grid" + std::to_string(nameCount++));


	for (int i = 0; i < lineVertexVector.size(); ++i)
	{
		uploadBuffer->CopyData(i, &lineVertexVector[i], sizeof(LineVertex));
	}
	grid->SetVertexBuffer(uploadBuffer);
	grid->SetVertexNum(lineVertexVector.size());

	

}


