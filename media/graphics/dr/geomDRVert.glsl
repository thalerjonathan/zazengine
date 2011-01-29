#version 330 core

in vec4 in_vertPos;
in vec4 in_vertNorm;

uniform mat4 in_modelViewProj;
uniform mat4 in_transform;

void main()
{
	gl_Position = in_modelViewProj * in_transform * in_vertPos;
}