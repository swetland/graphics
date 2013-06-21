#version 150
#extension GL_ARB_explicit_attrib_location : enable

-- vertex

layout(std140) uniform cb0 {
	mat4 MVP;
	int cw;
	int ch;
};

layout (location = 0) in vec4 POSITION;
layout (location = 1) in vec2 TEXCOORD;
layout (location = 2) in uint CHARACTER;

out vec2 vTEXCOORD;

void main() {
	vec4 pos = POSITION;
	int id = gl_InstanceID;

	// shift unit rectangle to character cell rectangle
	pos.xy += vec2(id % cw, (ch-1) - id / ch);

	// adjust unit texture coord to font cell rectangle
	float tx = (CHARACTER % uint(16));
	float ty = (CHARACTER / uint(16));

	vTEXCOORD =
		// scale to size of character in fonttexture
		TEXCOORD * vec2(1.0/16.0,1.0/16.0)
		// move to correct character
		+ vec2(tx/16.0,ty/16.0)
		// offset to avoid cruft
		+ vec2(1.0/256.0,1.0/256.0);


	pos = MVP * pos;
	// discard via clipping
	if (CHARACTER == uint(0)) pos.z = -1.1;

	gl_Position = pos;
}

-- fragment

uniform sampler2D uTexture0;

in vec2 vTEXCOORD;

void main() {
	gl_FragColor = texture2D(uTexture0, vTEXCOORD);
}
