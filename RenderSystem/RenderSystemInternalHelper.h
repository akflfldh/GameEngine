
#include"RenderType.h"


namespace Render
{											//상위16비트 - 다음 16비트  -다음 하위 32비트 
 	uint64_t GetRenderItemFirstSortKey(ScissorRect scissorRect, MaterialID matID);
    uint64_t GetRenderItemSecondSortKey(const MeshItem& meshItem, const InstanceItem& instanceItem);


    






}