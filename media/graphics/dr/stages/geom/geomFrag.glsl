#version 330 core

in vec4 ex_color;
in vec4 ex_shadowCoord;

out vec4 out_color;

out vec4 out_diffuse;
out vec4 out_normal;
out vec4 out_depth;
out vec4 out_generic;

uniform sampler2DShadow ShadowMap;

float lookup( float offsetX, float offsetY )
{
	float depth = textureProj( ShadowMap, ex_shadowCoord + vec4( offsetX, offsetY, 0.0, 0.005 ) );
	return depth != 1.0 ? 0.75 : 1.0; 
}


void main()
{
	out_color = ex_color * lookup( 0.0, 0.0 );

	out_diffuse = out_color;
	out_normal = vec4( 0.0, 1.0, 0.0, 1.0 );
	out_depth = vec4( 0.0, 0.0, 1.0, 1.0 );
	out_generic = vec4( 1.0, 1.0, 0, 1.0 );

}