#version 330 core

out vec4 out_color;

void main()
{
	out_color = vec4( 0.0, 0.0, gl_FragCoord.z, 1.0 );
}