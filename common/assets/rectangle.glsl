#version 140
#extension GL_ARB_explicit_attrib_location : enable

-- vertex

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

void main() {
	vTexCoord = aTexCoord;
	gl_Position = aPosition;
}

-- fragment

uniform sampler2DRect sampler0;

uniform float offset[3] =
	float[]( 0.0, 1.3846153846, 3.2307692308 );
uniform float weight[3] =
	float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

in vec2 vTexCoord;

layout (location = 0) out vec4 fColor;

void main() {
#ifdef SKIPLINES
	if ((int(gl_FragCoord.y) & 1) == 1) {
		discard;
		return;
	}
#endif
#ifdef SKIP2LINES
	if ((int(gl_FragCoord.y) & 3) < 2) {
		discard;
		return;
	}
#endif
#ifdef PIXELATE
	fColor = texture(sampler0, floor(gl_FragCoord.xy / 8.0) * 8.0);
#endif
#ifdef GRAY
	float gray = dot(texture(sampler0, gl_FragCoord.xy).rgb, vec3(0.299, 0.587, 0.114));
	fColor = vec4(gray, gray, gray, 1.0);
#endif
#ifdef GREEN
	float gray = dot(texture(sampler0, gl_FragCoord.xy).rgb, vec3(0.299, 0.587, 0.114));
	fColor = vec4(vec3(0.0, 1.0, 0.0) * gray, 1.0);
#endif
#ifdef HBLUR
	vec2 xy = gl_FragCoord.xy;
	vec4 c = texture(sampler0, xy / 1.0) * weight[0];
	for (int i = 1; i < 3; i++) {
		vec2 delta = vec2(offset[i], 0);
		c += texture(sampler0, (xy + delta) / 1.0) * weight[i];
		c += texture(sampler0, (xy - delta) / 1.0) * weight[i];
	}
	fColor = c;
#endif
#ifdef VBLUR
	vec2 xy = gl_FragCoord.xy;
	vec4 c = texture(sampler0, xy / 1.0) * weight[0];
	for (int i = 1; i < 3; i++) {
		vec2 delta = vec2(0, offset[i]);
		c += texture(sampler0, (xy + delta) / 1.0) * weight[i];
		c += texture(sampler0, (xy - delta) / 1.0) * weight[i];
	}
	fColor = c;
#endif
#ifdef COPY
	fColor = texture(sampler0, gl_FragCoord.xy);
#endif
}
