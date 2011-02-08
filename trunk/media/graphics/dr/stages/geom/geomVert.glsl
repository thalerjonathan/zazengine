#version 330 core

in vec3 in_vertPos;
in vec3 in_vertNorm;

out vec4 ex_color;
out vec4 ex_shadowCoord;

layout(shared) uniform transform
{
	mat4 mvp_mat;
	mat4 lightSpace_mat;
};

void main()
{
	ex_shadowCoord = lightSpace_mat * vec4( in_vertPos, 1.0 );
	ex_shadowCoord = ex_shadowCoord / ex_shadowCoord.w;
	
	ex_color = vec4( 1.0, 1.0, 1.0, 1.0 );

	gl_Position = mvp_mat * vec4( in_vertPos, 1.0 );
}
