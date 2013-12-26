#version 430 core

// restrict to max of 128 bones per mesh
#define MAX_BONES_PER_MESH 128

// inputs from the vertex-stream
layout( location = 0 ) in vec3 in_vertPos;
layout( location = 1 ) in vec3 in_vertNorm;
layout( location = 2 ) in vec2 in_texCoord;
layout( location = 3 ) in vec3 in_tangent;
layout( location = 4 ) in uint in_bone_count;
layout( location = 5 ) in uvec4 in_bone_indices;
layout( location = 6 ) in vec4 in_bone_weights;

// defines the output-interface block to the fragment-shader
out IN_OUT_BLOCK
{
	vec4 normal;
	vec2 texCoord;
	vec4 tangent;
	vec4 biTangent;
} IN_OUT;

// THE TRANSFORMATIONS FOR THE CURRENT MESH/VERTEX-STREAM
layout( shared ) uniform TransformUniforms
{
	// the model-matrix of the current rendered mesh 
	mat4 modelMatrix;
	// the model-view-matrix of the current rendered mesh - the view-matrix is the one of the Camera
	mat4 modelViewMatrix;
	// the model-view-projection matrix of the current rendered mesh - the view-projection-matrix is the one of the Camera
	mat4 modelViewProjMatrix;
} Transforms;

// the bones for skinning
uniform mat4 u_bones[ MAX_BONES_PER_MESH ];

// the prototype of the input-processing subroutines - all implementing subroutines copy the corresponding data into the IN_OUT block and returns the position
// application will select according whether a mesh is animated or not
// NOTE: directions have to be filled up with 0.0 as w because have no length
// NOTE: positions have to be filled up with 1.0 because have a length 
subroutine vec4 processInputs();
// subroutine selection-uniform
subroutine uniform processInputs processInputsSelection;

// skinning is performed
subroutine ( processInputs ) vec4 processInputsAnimated()
{
	vec4 position;

	for ( uint i = 0u; i < in_bone_count; i++ )
	{
		uint boneIndex = in_bone_indices[ i ];
		mat4 bone = u_bones[ boneIndex ];
		float boneWeight = in_bone_weights[ i ];

		position += boneWeight * ( bone * vec4( in_vertPos, 1.0 ) );
		IN_OUT.normal += boneWeight * ( bone * vec4( in_vertNorm, 0.0 ) );
		IN_OUT.tangent += boneWeight * ( bone * vec4( in_tangent, 0.0 ) );
	}

	return position;
}

// no animation performed, just copy
subroutine ( processInputs ) vec4 processInputsStatic()
{
	IN_OUT.normal = vec4( in_vertNorm, 0.0 );
	IN_OUT.tangent = vec4( in_tangent, 0.0 );

	// just pass through vertex-position
	return vec4( in_vertPos, 1.0 );
}

void main()
{
	// returns position transformed or static depending if mesh is animated or not
	vec4 position = processInputsSelection();

	// no transform for texture-coords, just interpolated accross vertices
	IN_OUT.texCoord = in_texCoord;

	// non-uniform scaling is forbidden in this engine therefore we can use modelViewMatrix for all directions
	IN_OUT.normal = Transforms.modelViewMatrix * IN_OUT.normal; 
	IN_OUT.tangent = Transforms.modelViewMatrix * IN_OUT.tangent;
	// construct bi-tangent from normal and tangent using the cross-product
	IN_OUT.biTangent = vec4( cross( IN_OUT.normal.xyz, IN_OUT.tangent.xyz ), 0.0 ); 
	 
	gl_Position = Transforms.modelViewProjMatrix * position;
}
