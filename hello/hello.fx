
matrix MVP;

Texture2D Texture0;

SamplerState samLinear {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_OUTPUT {
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VS_OUTPUT VS(
	float4 Pos : POSITION,
	float4 Normal : NORMAL,
	float2 TexCoord : TEXCOORD) {
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.Pos = mul(Pos,MVP); // DX
//	output.Pos = mul(MVP, Pos); // GL
	output.TexCoord = TexCoord;
	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target {
//	return float4(1.0,0.0,0.0,1.0);
	return Texture0.Sample(samLinear, input.TexCoord);
}

technique10 Render {
	pass P0 {
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}