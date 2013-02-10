
uniform sampler2D uTexture0;

in vec2 vTEXCOORD;

void main() {
	gl_FragColor = texture2D(uTexture0, vTEXCOORD);
}
