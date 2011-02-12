#version 330 core

in vec3 in_vertPos;

layout(shared) uniform mvp_transform
{
	mat4 mvp_mat;
	mat4 projInv_mat;
};

void main()
{
	gl_Position = mvp_mat * vec4( in_vertPos, 1.0 );
}
