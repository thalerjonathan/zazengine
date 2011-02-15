#version 330 core

in vec4 ex_shadowCoord;
in vec4 ex_depth;
in vec4 ex_normal;

out vec4 out_diffuse;
out vec4 out_depth;
out vec4 out_normal;

uniform sampler2DShadow ShadowMap;

layout(shared) uniform mvp_transform
{
	mat4 modelView_Matrix;
	mat4 modelViewProjection_Matrix;
	
	mat4 normalsModelView_Matrix;
	mat4 normalsModelViewProjection_Matrix;
	
	mat4 projection_Matrix;
	mat4 projectionInv_Matrix;
};

// contains light-direction in 8,9,10
layout(shared) uniform lightData
{
	mat4 light_ModelMatrix;
	mat4 light_SpaceMatrix;
	mat4 light_SpaceUnitMatrix;
};



vec4 packFloatToVec4i( const float value )
{
  const vec4 bitSh = vec4( 256.0 * 256.0 * 256.0, 256.0 * 256.0, 256.0, 1.0 );
  const vec4 bitMsk = vec4( 0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);
  vec4 res = fract( value * bitSh );
  res -= res.xxyz * bitMsk;
  return res;
}

float shadowLookup( const vec4 shadowCoord, const float offsetX, const float offsetY )
{
	return textureProj( ShadowMap, shadowCoord + vec4( offsetX, offsetY, 0.005, 0.0 ) );
}

void main()
{	
	float shadow = shadowLookup( ex_shadowCoord, 0.0, 0.0 );
	// this fragment is in shadow
	if ( shadow != 1.0 )
	{
		// encode shadow as red
		out_diffuse = vec4( 1.0, 0.0, 0.0, 1.0 );
	}
	else
	{
		// we need to transform the lights direction too when it should not stay with the camera
		// the problem with this approach is, that normalsModelView_Matrix is the matrix transforming
		// normals for the currently rendering object and this will not match the lights transforms
		vec3 lightDir = vec4( normalsModelView_Matrix * vec4( 0.0, 0.0, 1.0, 0.0 ) ).xyz;
		out_diffuse = vec4( 1.0, 1.0, 1.0, 1.0 ) * dot( ex_normal.xyz, lightDir );
	}
	
	out_depth = packFloatToVec4i( ex_depth.x / ex_depth.y );
	out_normal = ex_normal;
}