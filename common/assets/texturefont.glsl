#version 150
#extension GL_ARB_separate_shader_objects : enable

layout(std140) uniform udata0 {
	mat4 MVP;
	float dim;
};

-- vs

layout(location = 0) in ivec4 posn;
layout(location = 1) in ivec2 texc;
layout(location = 2) in vec4 color;

out vec2 size;
out vec2 tex;
out vec4 vcolor;

void main() {
	gl_Position = vec4(posn.x, posn.y, 0, 1);
	size = posn.zw;
	tex = texc;
	vcolor = color;
}

-- gs

layout (points) in;
layout (triangle_strip, max_vertices=4) out;

in vec2 size[1];
in vec2 tex[1];
in vec4 vcolor[1];

out vec2 s0tc;
out vec4 color;

void main() {
	vec4 p = gl_in[0].gl_Position;

	gl_Position = MVP * vec4(p.x, p.y - size[0].y, 0.0, 1.0);
	s0tc = tex[0] / dim;
	color = vcolor[0];
	EmitVertex();

	gl_Position = MVP * vec4(p.x + size[0].x, p.y - size[0].y, 0.0, 1.0);
	s0tc = (tex[0] + vec2(size[0].x,0)) / dim;
	color = vcolor[0];
	EmitVertex();

	gl_Position = MVP * vec4(p.x, p.y, 0.0, 1.0);
	s0tc = (tex[0] + vec2(0,size[0].y)) / dim;
	color = vcolor[0];
	EmitVertex();

	gl_Position = MVP * vec4(p.x + size[0].x, p.y, 0.0, 1.0);
	s0tc = (tex[0] + size[0]) / dim;
	color = vcolor[0];
	EmitVertex();

	EndPrimitive();
}

-- fs

uniform sampler2D s0glyphs;

in vec2 s0tc;
in vec4 color;

void main() {
	vec4 c = texture2D(s0glyphs, s0tc);
	gl_FragColor = vec4(color.r, color.g, color.b, c.r);
}
