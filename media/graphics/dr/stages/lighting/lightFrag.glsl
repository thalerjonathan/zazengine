#version 330 core

out vec4 out_final;

uniform sampler2D DiffuseMap;
uniform sampler2D DepthMap;
uniform sampler2D NormalMap;

uniform sampler2DShadow ShadowMap;

layout(shared) uniform mvp_transform
{
	mat4 mvp_mat;
	mat4 normal_mat;
	mat4 projInv_mat;
};

// contains light-direction in 8,9,10
layout(shared) uniform lightData
{
	mat4 lightSpace_mat;
};

float unpackFloatFromVec4i( const vec4 value )
{
  const vec4 bitSh = vec4( 1.0 / ( 256.0 * 256.0 * 256.0 ), 1.0 / ( 256.0 * 256.0 ), 1.0 / 256.0, 1.0 );
  return ( dot( value, bitSh ) );
}

vec3 positionFromDepth( const vec2 screenCoord, const float depth )
{
    // Get x/w and y/w (NDC) from the viewport position
    // after perspective division through w we are in the normaliced device coordinages 
    // which are in the range from -1 to +1.
    float ndcX = screenCoord.x * 2 - 1;
    float ndcY = ( 1 - screenCoord.y ) * 2 - 1;
    
    vec4 vProjectedPos = vec4( ndcX, ndcY, depth, 1.0f );
    
    // Transform by the inverse projection matrix
    vec4 pos = projInv_mat * vProjectedPos;
    
    // Divide by w to get the view-space position
    return pos.xyz / pos.w;  
}

float shadowLookup( const vec4 shadowCoord, const float offsetX, const float offsetY )
{
	return textureProj( ShadowMap, shadowCoord + vec4( offsetX, offsetY, 0.005, 0.0 ) );
}

void main()
{
	// 1. get the pixels normalized position on the screen (0-1) 
	vec2 screenCoord = vec2( gl_FragCoord.x / 800, gl_FragCoord.y / 600 );

	// 2. get the diffuse color
	vec4 diffuseComp = texture( DiffuseMap, screenCoord );

	// 3. get depth value
    float depth = unpackFloatFromVec4i( texture( DepthMap, screenCoord ) );  
	
	// 4. get the position of fragment in world-space
	vec4 fragWorldPos = vec4( positionFromDepth( screenCoord, depth ), 1.0 );

	// 5. transform the fragment world-pos to its position in light-space
	vec4 fragLightPos = lightSpace_mat * fragWorldPos;
	
	// 6. do the shadow lookup
	float shadow = shadowLookup( fragLightPos, 0.0, 0.0 );

	// this fragment is in shadow
	if ( shadow == 0.0 )
	{
		out_final = diffuseComp * 0.75;
		out_final.a = 1.0;
	}
	else
	{
		//vec4 normal = texture( NormalMap, screenCoord );
		//vec4 lightDir = lightSpace_mat[ 2 ];
		// out_final = diffuseComp * dot( normal, lightDir );
		out_final = diffuseComp;
		out_final.a = 1.0;
	}
}