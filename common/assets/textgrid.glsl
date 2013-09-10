#version 140
#extension GL_ARB_explicit_attrib_location : enable

-- vertex

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in uint aCharacter;

out vec2 vTexCoord;

void main() {
	ivec2 cell = uTextGrid.xy;
	ivec2 dims = uTextGrid.zw;
	vec4 pos = aPosition;
	int id = gl_InstanceID;

	// translate cell to destination 
	pos.xy += vec2(id % dims.x, id / dims.x) * cell;

	// adjust unit texture coord to font cell rectangle
	vec2 tadj = vec2(aCharacter % uint(16), aCharacter / uint(16)) / 16.0;

	// translate texture coordinates to character position
	vTexCoord = aTexCoord + tadj + vec2(1.0/256.0,1.0/256.0); 

	pos = uOrtho * pos;

	// discard via clipping
	if (aCharacter == uint(0)) pos.z = -1.1;

	gl_Position = pos;
}

-- fragment

uniform sampler2D uTexture0;

in vec2 vTexCoord;

void main() {
	gl_FragColor = texture2D(uTexture0, vTexCoord);
}
