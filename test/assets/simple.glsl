#version 140
#extension GL_ARB_explicit_attrib_location : enable

//#define TEXTURED
//#define SPECULAR

-- vertex

layout (location = A_POSITION) in vec4 aPosition;
layout (location = A_NORMAL) in vec3 aNormal;
layout (location = A_TEXCOORD) in vec2 aTexCoord;

out vec2 vTexCoord;
out vec3 vPosition; // eye space
out vec3 vNormal; // eye space

void main() {
	vPosition = (OBJ.MV * aPosition).xyz;
	vNormal = (OBJ.MV * vec4(aNormal, 0.0)).xyz;
	vTexCoord = aTexCoord;
        gl_Position = OBJ.MVP * aPosition;
}

-- fragment

in vec2 vTexCoord;
in vec3 vPosition;
in vec3 vNormal;

uniform sampler2D sampler0;

void main() {
#ifdef TEXTURED
	vec4 c = texture2D(sampler0, vTexCoord);
#else
        vec4 c = vec4(1.0, 0.0, 0.0, 1.0);
#endif
	vec3 n = normalize(vNormal);
	vec3 s;
	if (SCN.LightPosition.w > 0) {
		/* positional light, compute direction */
		s = normalize(SCN.LightPosition.xyz - vPosition);
	} else {
		/* directional light - light position is actually a vector */
		s = SCN.LightPosition.xyz;
	}
	vec3 v = normalize(-vPosition);
	vec3 h = normalize(v + s);

	gl_FragColor = MAT.Ambient * c
		+ MAT.Diffuse * c * max( dot(s, n), 0.0) 
#ifdef SPECULAR
		+ MAT.Specular * SCN.LightColor * pow( max( dot(h,n), 0.0), MAT.Shininess)
#endif
		;
}

