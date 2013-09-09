#version 140
#extension GL_ARB_explicit_attrib_location : enable

-- vertex

layout(location = A_POSITION) in vec4 aPosition;
layout(location = A_NORMAL) in vec3 aNormal;
layout(location = A_TEXCOORD) in vec2 aTexCoord;

layout(location = 3) in vec4 aOffset;

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;

void main() {
	vec4 pos = aPosition + aOffset * vec4(127.0, 127.0, 127.0, 0.0);
	vPosition = (uMV * pos).xyz;
	vNormal = normalize(uMV * vec4(aNormal, 0.0)).xyz;
	vTexCoord = aTexCoord;
	gl_Position = uMVP * pos;
}

-- fragment

in vec2 vTexCoord;
in vec3 vPosition;
in vec3 vNormal;

void main() {
        vec4 c = uColor;
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
