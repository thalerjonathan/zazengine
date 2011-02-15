#version 330 core

in vec3 in_vertPos;
in vec3 in_vertNorm;

out vec4 ex_depth;
out vec4 ex_normal;

layout(shared) uniform mvp_transform
{
	mat4 modelView_Matrix;
	mat4 modelViewProjection_Matrix;
	
	mat4 normalsModelView_Matrix;
	mat4 normalsModelViewProjection_Matrix;
	
	mat4 projection_Matrix;
	mat4 projectionInv_Matrix;
};

void main()
{
	gl_Position = modelViewProjection_Matrix * vec4( in_vertPos, 1.0 );
	ex_depth.xy = gl_Position.zw;
	ex_normal = normalsModelView_Matrix * vec4( in_vertNorm, 0.0 );
}
