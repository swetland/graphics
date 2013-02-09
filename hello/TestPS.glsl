#version 330

in vec2 vTEXCOORD;
in float vDIFFUSE;

void main() {
	vec4 c = vec4(1.0, 0.0, 0.0, 1.0);
	gl_FragColor = c * 0.25 + c * vDIFFUSE;
}
