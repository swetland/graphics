#version 330

layout(std140) uniform cb0 {
	mat4 MVP;
	mat4 MV;
};

in vec4 POSITION;
in vec4 NORMAL;
in vec2 TEXCOORD;
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
