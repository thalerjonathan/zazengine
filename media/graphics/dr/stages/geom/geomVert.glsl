#version 330 core

in vec3 in_vertPos;
in vec3 in_vertNorm;

out vec4 ex_shadowCoord;
//out vec4 ex_depth;
out vec4 ex_normal;

layout(shared) uniform mvp_transform
{
	mat4 modelView_Matrix;					// the model-view matrix for the mesh current rendererd
	mat4 modelViewProjection_Matrix;		// the model-view-projectoin matrix for the mesh current rendered
	
	mat4 normalsModelView_Matrix;			// the normals-model-view matrix for the mesh current rendered
	mat4 normalsModelViewProjection_Matrix;	
	
	mat4 projection_Matrix;					// the projection matrix currently set in the camera
	mat4 projectionInv_Matrix;				// the inverse projection matrix currently set in the camera
};

// contains light-direction in 8,9,10
layout(shared) uniform lightData
{
	mat4 light_ModelMatrix;
	mat4 light_SpaceMatrix;
	mat4 light_SpaceUnitMatrix;
};


void main()
{
	gl_Position = modelViewProjection_Matrix * vec4( in_vertPos, 1.0 );
	//ex_depth.xy = gl_Position.zw;
	ex_normal = normalsModelView_Matrix * vec4( in_vertNorm, 0.0 );
	ex_shadowCoord = light_SpaceUnitMatrix * vec4( in_vertPos, 1.0 );
}
