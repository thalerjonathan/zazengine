#version 330 core

in vec3 in_vertPos;

layout(shared) uniform transform
{
	mat4 mvp_mat;
	mat4 lightSpace_mat;
};

void main()
{
	gl_Position = mvp_mat * vec4( in_vertPos, 1.0 );
}
