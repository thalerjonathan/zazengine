#version 330 core

in vec4 ex_depth;
in vec4 ex_shadowCoord;

out vec4 out_diffuse;
out vec4 out_depth;

uniform sampler2DShadow ShadowMap;

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
	float shadowed = shadowLookup( ex_shadowCoord, 0.0, 0.0 );
	if ( shadowed == 0.0 )
	{
		out_diffuse = vec4( 0.75, 0.75, 0.75, 1.0 );
	}
	else
	{
		out_diffuse = vec4( 1.0, 1.0, 1.0, 1.0 );
	}
		
	out_depth = packFloatToVec4i( ex_depth.x / ex_depth.y );
}