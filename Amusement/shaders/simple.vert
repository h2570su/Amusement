#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinate;
layout (location = 3) in vec3 color;

uniform mat4 u_model;

layout (std140, binding = 0) uniform commom_matrices
{
    mat4 u_projection;
    mat4 u_view;
};


out vec3 o_position;
out vec3 o_normal;
out vec2 o_texture_coordinate;
out vec3 o_color;


void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(position, 1.0f);	
    o_position = vec3(u_model * vec4(position, 1.0f));
    o_normal = mat3(transpose(inverse(u_model))) * normal;
    o_texture_coordinate = vec2(texture_coordinate.x, 1.0f - texture_coordinate.y);
	o_color = color;
}