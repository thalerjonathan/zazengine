#version 330 core

out vec4 out_final;

uniform sampler2D DiffuseMap;
uniform sampler2D DepthMap;
uniform sampler2DShadow ShadowMap;

layout(shared) uniform mvp_transform
{
	mat4 mvp_mat;
	mat4 projInv_mat;
};

layout(shared) uniform lightData
{
	mat4 lightSpace_mat;
};


float unpackFloatFromVec4i( const vec4 value )
{
  const vec4 bitSh = vec4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1.0);
  return(dot(value, bitSh));
}

// Function for converting depth to view-space position
// in deferred pixel shader pass.  vTexCoord is a texture
// coordinate for a full-screen quad, such that x=0 is the
// left of the screen, and y=0 is the top of the screen.
vec3 positionFromDepth( const vec2 vTexCoord, const float depth )
{
    // Get x/w and y/w from the viewport position
    float x = vTexCoord.x * 2 - 1;
    float y = ( 1 - vTexCoord.y ) * 2 - 1;
    vec4 vProjectedPos = vec4( x, y, depth, 1.0f );
    
    // Transform by the inverse projection matrix
    vec4 vPositionVS = projInv_mat * vProjectedPos;
    
    // Divide by w to get the view-space position
    return vPositionVS.xyz / vPositionVS.w;  
}

float shadowLookup( const vec4 shadowCoord, const float offsetX, const float offsetY )
{
	return textureProj( ShadowMap, shadowCoord + vec4( offsetX, offsetY, 0.005, 0.0 ) );
}

void main()
{
	// 1. get the pixels normalized position on the screen (0-1) 
	vec2 normalizedScreenCoord = vec2( 1.0, 1.0 );

	// 2. get the diffuse color
	vec4 diffuseComp = texture( DiffuseMap, normalizedScreenCoord );
	
	// 3. get depth value
    float depth = unpackFloatFromVec4i( texture( DepthMap, normalizedScreenCoord ) );  
	
	// 4. get the position of fragment in world-space
	vec4 fragWorldPos = vec4( positionFromDepth( normalizedScreenCoord, depth ), 1.0 );

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
		out_final = diffuseComp;
		out_final.a = 1.0;
	}
}