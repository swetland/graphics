#version 140
#extension GL_ARB_explicit_attrib_location : enable

#define TEXTURED 1 

-- vertex

layout(std140) uniform block0 {
        mat4 MVP;
        mat4 MV;
};

layout (location = 0) in vec4 POSITION;
layout (location = 1) in vec4 NORMAL;
layout (location = 2) in vec2 TEXCOORD;

out vec2 vTEXCOORD;
out float vDIFFUSE;
out vec4 vCOLOR;

void main() {
        vec4 pos = POSITION;

        vec3 mvPosition = (MV * pos).xyz;
        vec3 mvNormal = (MV * vec4(NORMAL.xyz,0.0)).xyz;

        vec3 lightVec = normalize(vec3(10,20,25) - mvPosition);
        float diffuse = max(dot(mvNormal, lightVec), 0.0);

        gl_Position = MVP * POSITION;
        vTEXCOORD = TEXCOORD;
        vDIFFUSE = diffuse;
}

-- fragment

in vec2 vTEXCOORD;
in float vDIFFUSE;

uniform sampler2D sampler0;

void main() {
#if TEXTURED
	gl_FragColor = texture2D(sampler0, vTEXCOORD);
#else
        vec4 c = vec4(1.0, 0.0, 0.0, 1.0);
        gl_FragColor = c * 0.25 + c * vDIFFUSE;
#endif
}

