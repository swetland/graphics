
matrix MVP;

struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

VS_OUTPUT VS( float4 Pos : POSITION, float4 Color : COLOR ) {
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.Pos = mul(Pos,MVP); // DX
//	output.Pos = mul(MVP, Pos); // GL
	output.Color = Color;
	return output;
}

float4 PS( VS_OUTPUT input ) : SV_Target {
    return input.Color;
}

technique10 Render {
	pass P0 {
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}