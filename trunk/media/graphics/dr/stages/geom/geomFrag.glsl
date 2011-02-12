#version 330 core

in vec4 ex_depth;

out vec4 out_diffuse;
out vec4 out_depth;

vec4 packFloatToVec4i( const float value )
{
  const vec4 bitSh = vec4( 256.0 * 256.0 * 256.0, 256.0 * 256.0, 256.0, 1.0 );
  const vec4 bitMsk = vec4( 0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);
  vec4 res = fract( value * bitSh );
  res -= res.xxyz * bitMsk;
  return res;
}

void main()
{
	// for now we just write white as diffuse color
	out_diffuse = vec4( 1.0, 1.0, 1.0, 1.0 );
	out_depth = packFloatToVec4i( ex_depth.x / ex_depth.y );

}