
varying vec2 vTEXCOORD;
varying float vDIFFUSE;

void main() {
	vec4 c = vec4(1.0, 0.0, 0.0, 1.0);
	gl_FragColor = c * 0.25 + c * vDIFFUSE;
}
