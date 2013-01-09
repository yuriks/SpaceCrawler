#version 130

in vec2 vf_tex_coord;
flat in vec4 vf_color;

out vec4 out_color;

uniform sampler2D u_texture;

void main() {
	out_color = vf_color * texture(u_texture, vf_tex_coord);
}
