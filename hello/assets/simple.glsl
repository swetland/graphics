#version 140
#extension GL_ARB_explicit_attrib_location : enable

-- vertex

layout(std140) uniform cb0 {
	mat4 MVP;
	mat4 MV;
};

layout(location = 0) in vec4 POSITION;
layout(location = 1) in vec4 NORMAL;
layout(location = 2) in vec2 TEXCOORD;

in vec4 LOCATION;

out vec2 vTEXCOORD;
out float vDIFFUSE;

void main() {
	vec4 pos = POSITION;

	pos.xyz += LOCATION.xyz * vec3(127,127,127);

	vec3 mvPosition = (MV * pos).xyz;
	vec3 mvNormal = (MV * vec4(NORMAL.xyz,0.0)).xyz;

	vec3 lightVec = normalize(vec3(10,20,25) - mvPosition);
	float diffuse = max(dot(mvNormal, lightVec), 0.0);

	gl_Position = MVP * pos; //POSITION;
	vTEXCOORD = TEXCOORD;
	vDIFFUSE = diffuse;
}

-- fragment

varying vec2 vTEXCOORD;
varying float vDIFFUSE;

void main() {
	vec4 c = vec4(1.0, 0.0, 0.0, 1.0);
	gl_FragColor = c * 0.25 + c * vDIFFUSE;
}
