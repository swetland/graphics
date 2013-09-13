#version 140
#extension GL_ARB_explicit_attrib_location : enable

-- vertex

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in uvec4 aCharacter;

out vec2 vTexCoord;
out vec3 vColor;

void main() {
	ivec2 cell = uTextGrid.xy;
	ivec2 dims = uTextGrid.zw;
	vec4 pos = vec4(aPosition,0,1);
	int id = gl_InstanceID;

	// translate cell to destination 
	pos.xy += vec2(id % dims.x, id / dims.x) * cell;

	// adjust unit texture coord to font cell rectangle
	vec2 tadj = vec2(aCharacter.x % uint(16), aCharacter.x / uint(16)) / 16.0;

	// translate texture coordinates to character position
	vTexCoord = aTexCoord + tadj + vec2(1.0/256.0,1.0/256.0); 
	vColor = vec3(aCharacter.yzw) / 255.0;
	pos = uOrtho * pos;

	// discard via clipping
	if (aCharacter.x == uint(0)) pos.z = -1.1;

	gl_Position = pos;
}

-- fragment

uniform sampler2D sampler0;

in vec2 vTexCoord;
in vec3 vColor;

void main() {
	float alpha = texture2D(sampler0, vTexCoord).r;
	gl_FragColor = vec4(vColor.xyz * step(1.0,alpha), alpha);
}
