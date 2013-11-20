#version 400 core

in vec3 in_vertPos;
in vec3 in_vertNorm;
in vec2 in_texCoord;

out vec4 ex_pos;
out vec4 ex_normal;
out vec2 ex_texCoord;

// THE CAMERA CONFIGURATION FOR THE CURRENT VIEW
layout( shared ) uniform CameraUniforms
{
	// the width (x) and height (y) of the camera-window in pixels ( the resolution )
	vec2 window;	
	// the near- (x) and far-plane distances (y)
	vec2 nearFar;

	// the model-matrix of the camera (orienation within world-space)
	mat4 modelMatrix;
	// the view-matrix of the camera to apply to the objects to transform to view/eye/camera-space (is its inverse model-matrix)
	mat4 viewMatrix;
	// the projection-matrix of the camera
	mat4 projectionMatrix;
} Camera;

// TRANSFORMATIONS FOR THE CURRENT MESH/VERTEX-STREAM
layout( shared ) uniform TransformUniforms
{
	// the model-matrix of the current rendered mesh - transform to world-space
	mat4 modelMatrix;
	// the model-view-matrix of the current rendered mesh - the view-matrix is the one of the Camera - transforms from model-space to view/eye/camera-space
	mat4 modelViewMatrix;
} Transforms;

void main()
{
	ex_normal = Transforms.modelMatrix * vec4( in_vertNorm, 0.0 );
	// no transform for texture-coords, just interpolated
	ex_texCoord = in_texCoord;

	// OPTIMIZE: premultiply projection & modelView on CPU 
	gl_Position = Camera.projectionMatrix * Transforms.modelViewMatrix * vec4( in_vertPos, 1.0 );
	
	ex_pos = gl_Position;
}
