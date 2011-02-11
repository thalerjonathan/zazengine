#version 330 core

in vec3 in_vertPos;
in vec3 in_vertNorm;

out vec4 ex_normal;

layout(shared) uniform transform
{
	mat4 mvp_mat;
	mat4 lightSpace_mat;
};

void main()
{
	ex_normal = vec4( in_vertNorm, 1.0 );

	gl_Position = mvp_mat * vec4( in_vertPos, 1.0 );
}
