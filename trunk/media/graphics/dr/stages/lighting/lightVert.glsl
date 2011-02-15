#version 330 core

in vec3 in_vertPos;

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
	// only apply projection-matrix because we do orthogonal quad rendering and apply no modeling
	gl_Position = projection_Matrix * vec4( in_vertPos, 1.0 );
}
