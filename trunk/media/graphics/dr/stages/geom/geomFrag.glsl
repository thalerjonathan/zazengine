#version 330 core

in vec4 ex_normal;

out vec4 out_diffuse;
out vec4 out_normal;
out vec4 out_depth;
out vec4 out_generic;

void main()
{
	out_diffuse = vec4( 1.0, 1.0, 1.0, 1.0 );
	out_normal = ex_normal;
	out_depth = vec4( 0.0, 0.0, 1.0, 1.0 );
	out_generic = vec4( 1.0, 0.0, 0, 1.0 );

}