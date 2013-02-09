
Texture2D Texture0;

SamplerState samLinear;

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_Target {
	return Texture0.Sample(samLinear, input.TexCoord);
}