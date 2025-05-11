#pragma once



class Object;

#include<vector>
#include<queue>
//#include<windef.h>
#include<windows.h>
#include"SpacePartitioningStructure.h"


//지역도 바운딩볼륨으로 나타낸다.
//그리고 object들을 추가할때마다 어느지역에 추가해야할지는 그 object의 bounding volume과 지역의 bouding volume을
//교차판정을해서 결정할수있다.

//또 정적모델들을 위한 공간분할구조(월드와), 동적모델들을 위한 공간분할구조(월드) 를 각각 가지자.

namespace Quad
{
	/*class QuadTree :public SpacePartitioningStructure
	{



	};*/

}