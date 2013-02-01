
cbuffer cb0 {
	matrix MVP;
}

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VS_OUTPUT main(
	float4 Position : POSITION,
	float4 Normal	: NORMAL,
	float2 TexCoord : TEXCOORD) {
	VS_OUTPUT output = (VS_OUTPUT) 0;
//	output.Position = mul(Position, MVP); // DX
	output.Position = mul(MVP, Position); // GL
	output.TexCoord = TexCoord;
	return output;
}
