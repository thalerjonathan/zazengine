#version 330 core

in vec3 in_vertPos;
in vec3 in_vertNorm;

out vec4 ex_depth;

layout(shared) uniform mvp_transform
{
	mat4 mvp_mat;
	mat4 projInv_mat;
};

void main()
{
	gl_Position = mvp_mat * vec4( in_vertPos, 1.0 );
	ex_depth.xy = gl_Position.zw;
}
