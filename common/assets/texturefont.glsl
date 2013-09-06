#version 140
#extension GL_ARB_explicit_attrib_location : enable

uniform samplerBuffer sampler0; // character data
uniform sampler2D sampler1; // glyph texture

layout(std140) uniform block3 {
	mat4 xMVP;
};

-- vs

layout(location = 0) in ivec4 aData; // X, Y, ID, RGBA

out vec2 vTexCoord;
out vec4 vColor;

void main() {
	vec4 cdata = texelFetch(sampler0, aData.z + gl_VertexID);
	vec4 pos = vec4(aData.x + cdata.x, aData.y + cdata.y, 0.0, 1.0);
	vColor = vec4(
			float(aData.w & 0xFF) / 255.0,
			float((aData.w >> 8) & 0xFF) / 255.0,
			float((aData.w >> 16) & 0xFF) / 255.0,
			1.0
		);
	vTexCoord = vec2(cdata.zw);
	gl_Position = xMVP * pos;
}

-- fs

in vec2 vTexCoord;
in vec4 vColor;

void main() {
	vec4 c = texture2D(sampler1, vTexCoord);
	gl_FragColor = vec4(vColor.rgb, c.r);
}
