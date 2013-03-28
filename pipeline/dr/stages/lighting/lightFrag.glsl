#version 330 core

uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D DepthMap;
uniform sampler2D GenericMap1;

uniform sampler2DShadow ShadowMap;

out vec4 final_color;

layout( shared ) uniform camera
{
	mat4 camera_Model_Matrix;			// 0
	mat4 camera_View_Matrix;			// 64
	
	vec4 camera_rec;					// 128
};

layout( shared ) uniform light
{
	vec4 light_Config; 				// x: type, y: falloff, z: shadowCaster 0/1
	vec4 light_Color;

	mat4 light_Model_Matrix;
	mat4 light_SpaceUniform_Matrix;
};

vec4
calculateLambertian( in vec4 diffuse, in vec4 normal, in vec4 position )
{
	// need to transpose light-model matrix to view-space for eye-coordinates 
	// OPTIMIZE: premultiply on CPU
	mat4 lightMV_Matrix = camera_View_Matrix * light_Model_Matrix;

	// light-position is stored in 4th vector
	vec3 lightPos = vec3( lightMV_Matrix[ 3 ] );
	vec3 lightDir = normalize( lightPos - vec3( position ) );
    
	// calculate attenuation-factor
	float attenuationFactor = max( dot( vec3( normal ), lightDir ), 0.0 );

	return attenuationFactor * diffuse;
}

vec4
calculatePhong( in vec4 diffuse, in vec4 normal, in vec4 position )
{
	// need to transpose light-model matrix to view-space for eye-coordinates 
	// OPTIMIZE: premultiply on CPU
	mat4 lightMV_Matrix = camera_View_Matrix * light_Model_Matrix;

	vec3 lightPos = vec3( lightMV_Matrix[ 3 ] );
	vec3 lightDir = normalize( lightPos - vec3( position ) );
    
	vec3 eyeDir = normalize( vec3( camera_Model_Matrix[ 3 ] ) - vec3( position ) );
	vec3 vHalfVector = normalize( lightDir + eyeDir );
    
	// calculate attenuation-factor
	float attenuationFactor = max( dot( vec3( normal ), lightDir ), 0.0 );

	float specularFactor =  pow( max( dot( vec3( normal ), vHalfVector ), 0.0 ), 100 ) * 1.5;

	return attenuationFactor * diffuse + specularFactor;    
}

void main()
{
	// fetch the coordinate of this fragment in normalized
	// screen-space ( 0 – 1 ) 
	vec2 screenCoord = vec2( gl_FragCoord.x / camera_rec.x, gl_FragCoord.y / camera_rec.y );

	vec4 diffuse = texture( DiffuseMap, screenCoord );

	// IMPORTANT: need to normalize normals due to possible uniform-scaling applied to models
	// OPTIMIZE: apply SCALE-Matrix only to modelView and not to normalsModelView
	vec4 normal = normalize( texture( NormalMap, screenCoord ) );

	// position of fragment is stored in model-view coordinates = EyeCoordinates (EC)
	// EC is what we need for lighting-calculations
	vec4 ecPosition = texture( GenericMap1, screenCoord );

	// for shadow-mapping we need to transform the position of the fragment to light-space
	// before we can apply the light-space transformation we first need to apply
	// the inverse view-matrix of the camera to transform the position back to world-coordinates (WC)
	// note that world-coordinates is the position after the modeling-matrix was applied to the vertex
	// OPTIMIZE: precalculate inverse camera-view matrix on CPU
	vec4 wcPosition = inverse( camera_View_Matrix ) * ecPosition;
	vec4 shadowCoord = light_SpaceUniform_Matrix * wcPosition;

	float shadow = 0.0;
	float bias = 0.0001;

	shadowCoord.z -= bias;

	// spot-light - do perspective shadow-lookup
	if ( 0.0 == light_Config.x )
	{
		// IMPORTANT: we are only in clip-space, need to divide by w to reach projected NDC.
		// normally the forward-rendering shadow-mapping calculates the shadow-coord
		// in the vertex-shader, which is not possible in the deferred renderer without using 
		// an additional render-target. in forward-rendering between the vertex-shader
		// and the fragment-shader where the shadow-map lookup happens
		// interpolation & perspective division is carried out by the fixed-function
		// so we need to do this here in the fragment-shader of the deferred renderer as well
		// ADDITION: either use 	
		//		vec3 shadowCoordPersp = shadowCoord.xyz / shadowCoord.w; with texture( ShadowMap, shadowCoordPersp ) lookup 
		//		OR use textureProj( ShadowMap, shadowCoord ); directly
		
		// IMPORTANT: because we installed a compare-function on this shadow-sampler
		// we don't need to compare it anymore to the z-value of the shadow-coord
		shadow = textureProj( ShadowMap, shadowCoord );
	}
	else if ( 1.0 == light_Config.x )
	{
		// IMPORTANT: directionaly light uses orthogonal shadow-map so 
		// transformation of position to shadow-coord is orthgonal projection too
		// so no need for perspective lookup (or divide) because orthogonal 
		// projection has 1 at w so there won't be a foreshortening of values
		// IMPORTANT: because we installed a compare-function on this shadow-sampler
		// we don't need to compare it anymore to the z-value of the shadow-coord
		shadow = texture( ShadowMap, shadowCoord );
	}

	// not in shadow
	if ( shadow != 0.0 )
	{
		float matId = diffuse.a * 255;

		if ( 1.0 == matId )
		{
			final_color = calculateLambertian( diffuse, normal, ecPosition );
		}
		else if ( 2.0 == matId )
		{
			final_color = calculatePhong( diffuse, normal, ecPosition );
		}
		else
		{
			final_color = diffuse;
		}
	}
	else
	{
		final_color = calculateLambertian( diffuse, normal, ecPosition );
		final_color.a = 1.0;
	}
}
