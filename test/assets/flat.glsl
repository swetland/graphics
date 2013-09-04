#version 140
#extension GL_ARB_explicit_attrib_location : enable

-- vertex

layout (location = A_POSITION) in vec4 aPosition;

void main() {
	gl_Position = OBJ.MVP * aPosition;
}

-- fragment

void main() {
	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
