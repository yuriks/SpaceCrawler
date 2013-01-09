#version 130

in vec2 in_position;
in vec2 in_tex_coord;
in vec4 in_color;

out vec2 vf_tex_coord;
flat out vec4 vf_color;

uniform mat3 u_view_matrix;

void main() {
	vf_tex_coord = in_tex_coord;
	vf_color = in_color;
	gl_Position = vec4(u_view_matrix * vec3(in_position.xy, 1), 1);
}
