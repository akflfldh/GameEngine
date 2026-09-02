cbuffer PassBuffer :register(b0)
{
float4x4 gViewProj;
float3 gEye;
}


struct VertexIn
{
    float2 mPos : POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
    float mPxRange : COMMON;
    float mCommonTwo : COMMON1;
    float mCommonThree : COMMON2;
};

struct VertexOut
{
    float4 mPosH :SV_POSITION;
    float3 mPosW: POSITION;
}

VertexOut VS(uint vertexID : SV_VertexID)
{
    float scale = 10000.0f;


    float2 gridCorners[4] = {
        float2(-scale,  scale), // 0번 정점 (좌상단)
        float2( scale,  scale), // 1번 정점 (우상단)
        float2(-scale, -scale), // 2번 정점 (좌하단)
        float2( scale, -scale)  // 3번 정점 (우하단)
    };

    float3 worldPos = float3(
        gEye.x + gridCorners[vertexID].x,
        0.0f, // 격자는 바닥이니까 Y는 0
        gEye.z + gridCorners[vertexID].y
    );


VertexOut vout;
vout.mPosW =worldPos;
vout.mPosH =mul(float4(worldPos,1.0f),gViewProj);

return vout;

}


float DrawGrid(float2 worldPos, float gridSize)
{
    //현재픽셀의 월드좌표를 그리드 단위의좌표로 변환 
    float2 coord = worldPos/gridSize;


    //픽셀사이의 월드좌표(그리드단위)변화율을 계산 
    float2 derivative  = fwidth(coord);

    // 정수값을 중심으로 -0.5 0.5 사이의값들에대해서  정수는 0 , 양 끝은 최대 0.5
    //를 가지는 중심에서 벌어진 정도를 나타내는  v 모양으로 변환하는 작업
    //마지막으로 픽셀단위로 변환 (0을 기준으로 떨어진 픽셀 거리 정보를 얻는다 )
    float2 grid = abs(frac(coord-0.5f)-0.5f) / derivative ;
    
    //x축,y축선중 좀 더 진한것(정수에 가까운것선택)
    float Line = min(grid.x, grid.y);

//1픽셀 이내의 값들은 부드럽게 블러차리 , 1픽셀을 넘어가는 경우에는 완전 투명 
    return 1.0f-min(Line,1.0f);
}


float4 PS(VertexOut pin) :SV_Target{

float thinLine = DrawGrid(pin.mPosW.xz,1.0f);

float thickLine= DrawGrid(pin.mPosW.xz, 10.0F);

float dist = length(pin.mPosW -  gEye);

float fade = 1.0f- saturate(dist/100.0f); //100 이하 

float3 gridColor = float3(0.5f,0.5f,0.5f);

//굵은라인은 더 찐하게 
float alpha = max(thinLine * 0.3f, thickLine * 0.8f) * fade;

return float4(gridColor,alpha);

}
