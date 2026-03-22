//cbuffer cbuf
//{
//    float4 g_vQuadRect;
//}

//struct Fragment
//{
//    float4 Pos : SV_POSITION;
//    float3 Tex : TEXCOORD0;
//};

//Fragment VS(uint vertexId : SV_VertexID)
//{
//    Fragment f;
//    f.Tex = float3(0.f, 0.f, 0.f);
//    if (vertexId == 1)
//        f.Tex.x = 1.f;
//    else if (vertexId == 2)
//        f.Tex.y = 1.f;
//    else if (vertexId == 3)
//        f.Tex.xy = float2(1.f, 1.f);
    
//    f.Pos = float4(g_vQuadRect.xy + f.Tex.xy * g_vQuadRect.zw, 0, 1);
    
//    return f;
//}