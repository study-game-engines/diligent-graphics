"#include \"FullScreenTriangleVSOutput.fxh\"\n"
"\n"
"FullScreenTriangleVSOutput FullScreenTriangleVS(in uint VertexId : SV_VertexID,\n"
"                                                in uint InstID   : SV_InstanceID)\n"
"{\n"
"    FullScreenTriangleVSOutput VSOut;\n"
"  \n"
"    float2 PosXY[3];\n"
"    PosXY[0] = float2(-1.0, -1.0);\n"
"    PosXY[1] = float2(-1.0, +3.0);\n"
"    PosXY[2] = float2(+3.0, -1.0);\n"
"\n"
"    float2 f2XY = PosXY[VertexId];\n"
"    VSOut.f2NormalizedXY = f2XY;\n"
"    VSOut.fInstID = float( InstID );\n"
"    \n"
"    // Write 0 to the depth buffer\n"
"    // NDC_MIN_Z ==  0 in DX\n"
"    // NDC_MIN_Z == -1 in GL\n"
"    float z = NDC_MIN_Z;\n"
"    VSOut.f4PixelPos = float4(f2XY, z, 1.0);\n"
"\n"
"    return VSOut;\n"
"}\n"
