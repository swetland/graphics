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
	vPosition = (uMV * aPosition).xyz;
	vNormal = (uMV * vec4(aNormal, 0.0)).xyz;
	vTexCoord = aTexCoord;
        gl_Position = uMVP * aPosition;
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
        vec4 c = uColor;
#endif
	vec3 n = normalize(vNormal);
	vec3 s;
	if (uLightPosition.w > 0) {
		/* positional light, compute direction */
		s = normalize(uLightPosition.xyz - vPosition);
	} else {
		/* directional light - light position is actually a vector */
		s = uLightPosition.xyz;
	}
	vec3 v = normalize(-vPosition);
	vec3 h = normalize(v + s);

	gl_FragColor = uAmbient * c
		+ uDiffuse * c * max(dot(s, n), 0.0) 
#ifdef SPECULAR
		+ uSpecular * uLightColor * pow( max( dot(h,n), 0.0), uShininess)
#endif
		;
}

