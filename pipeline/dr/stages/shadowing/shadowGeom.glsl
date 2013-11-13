#version 400 core
 
layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;
 
 void main()
{
	for( int i = 0; i < gl_in.length(); i++ )
	{
		// copy attributes
		gl_Position = gl_in[ i ].gl_Position;
 
		// done with the vertex
		EmitVertex();
	}
}