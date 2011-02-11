#version 330 core

out vec4 out_final;

uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D DepthMap;
uniform sampler2D GenericMap;

uniform sampler2DShadow ShadowMap;

float lookup( vec4 shadowCoord )
{
	//float depth = textureProj( ShadowMap, ex_shadowCoord + vec4( offsetX, offsetY, 0.0, 0.005 ) );
	float depth = textureProj( ShadowMap, shadowCoord );
	return depth != 1.0 ? 0.75 : 1.0; 
}

void main()
{
	vec2 screenSpaceCoord = vec2( 1.0, 1.0 );
	vec4 diffuseComp = texture( DiffuseMap, screenSpaceCoord );
	vec4 normalComp = texture( NormalMap, screenSpaceCoord );
	vec4 depthComp = texture( DepthMap, screenSpaceCoord );
	vec4 genericComp = texture( GenericMap, screenSpaceCoord );
				
	out_final = vec4( 1.0, 0.0, 0.0, 1.0 );
}