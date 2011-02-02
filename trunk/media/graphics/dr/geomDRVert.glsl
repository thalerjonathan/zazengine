#version 330 core

in vec4 in_vertPos;
in vec4 in_vertNorm;

layout(shared) uniform transform
{
    mat4 mvp_mat;
};

void main()
{
	gl_Position = mvp_mat * in_vertPos;
}