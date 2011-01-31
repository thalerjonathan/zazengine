#version 330 core

in vec4 in_vertPos;
in vec4 in_vertNorm;

uniform transform_mat
{
    mat4 modelViewProj;
    mat4 transform;
};

void main()
{
	gl_Position = modelViewProj * transform * in_vertPos;
}