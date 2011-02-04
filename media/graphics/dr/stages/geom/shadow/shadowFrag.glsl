#version 330 core

in vec3 ex_color;
out vec4 out_color;

/*
out vec4 out_diffuse;
out vec4 out_normal;
out vec4 out_depth;
out vec4 out_generic;
*/

void main()
{
	out_color = vec4( ex_color, 1.0 );
	
/*
	out_diffuse = vec4(1, 0, 0, 1);
	out_normal = vec4(1, 0, 0, 1);
	out_depth = vec4(1, 0, 0, 1);
	out_generic = vec4(1, 0, 0, 1);
*/	
}