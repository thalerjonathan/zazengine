#version 330 core

in vec3 in_vertPos;
in vec3 in_vertNorm;
in vec2 in_texCoord;

out vec4 ex_position;
out vec4 ex_normal;
out vec2 ex_texCoord;

layout( shared ) uniform TransformUniforms
{
	mat4 modelViewMatrix;
	mat4 projectionMatrix;

	mat4 normalsModelViewMatrix;
} Transforms;

void main()
{
	// IMPORTANT: out-variables are interepolated but no perspective division is applied

	// store position in view-space (EyeCoordinates) 
	ex_position = Transforms.modelViewMatrix * vec4( in_vertPos, 1.0 );
	// store normals in view-space too (EC)
	ex_normal = Transforms.normalsModelViewMatrix * vec4( in_vertNorm, 0.0 );
	// no transform for texture-coords, just interpolated
	ex_texCoord = in_texCoord;

	// OPTIMIZE: premultiply projection & modelView on CPU 
	// calculate position of vertex using MVP-matrix. 
	// will then be in clip-space after this transformation is applied
	// clipping will be applied
	// then opengl will apply perspective division 
	// after this the coordinates will be between -1 to 1 which is NDC
	// then view-port transform will happen
	// then fragment-shader takes over
	gl_Position = Transforms.projectionMatrix * Transforms.modelViewMatrix * vec4( in_vertPos, 1.0 );
}
