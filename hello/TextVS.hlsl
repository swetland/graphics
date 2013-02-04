
cbuffer cb0 {
	matrix MVP;
	unsigned int cw;
	unsigned int ch;
};
struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VS_OUTPUT main(
	float4 pos : POSITION,
	float2 uv : TEXCOORD,
	unsigned int c : CHARACTER,
	unsigned int id : SV_INSTANCEID
	) {

	VS_OUTPUT output = (VS_OUTPUT) 0;

	// shift unit rectangle to character cell rectangle
	pos.xy += float2(id % cw, (ch-1) - id / ch);

	// adjust unit texture coord to font cell rectangle
	float tx = (c % 16);
	float ty = (15 - c / 16);

	output.Position = mul(MVP, pos); 
	output.TexCoord =
		// scale to size of character in fonttexture
		uv * float2(1.0/16.0,1.0/16.0)
		// move to correct character
		+ float2(tx/16.0,ty/16.0)
		// offset to avoid cruft
		+ float2(1.0/256.0,-1.0/256.0);

	// discard via clipping
	if (c == 0) output.Position.z = -1.1;

	return output;
}
