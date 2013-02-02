
cbuffer cb0 {
	matrix MVP;
	matrix MV;
}

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float Diffuse : DIFFUSE;
};

VS_OUTPUT main(
	float4 Position : POSITION,
	float4 Normal	: NORMAL,
	float2 TexCoord : TEXCOORD,
	float4 Location : LOCATION) {

	VS_OUTPUT output = (VS_OUTPUT) 0;
	Position.xyz += Location.xyz;

	float3 mvPosition = mul(MV, Position).xyz;
	float3 mvNormal = mul(MV, float4(Normal.xyz,0.0)).xyz;

	float3 lightVec = normalize(float3(10,20,25) - float3(0,0,0));
	float diffuse = max(dot(mvNormal, lightVec), 0.0);

	output.Position = mul(MVP, Position);
	output.TexCoord = TexCoord;
	output.Diffuse = diffuse;

	return output;
}
