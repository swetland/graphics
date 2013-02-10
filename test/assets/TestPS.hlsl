
//Texture2D Texture0;
//SamplerState samLinear;

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float Diffuse : DIFFUSE;
};

float4 main(VS_OUTPUT input) : SV_Target {
//	return Texture0.Sample(samLinear, input.TexCoord);
	float4 c = float4(1.0, 0.0, 0.0, 1.0);
	return c * 0.25 + c * input.Diffuse;
}
