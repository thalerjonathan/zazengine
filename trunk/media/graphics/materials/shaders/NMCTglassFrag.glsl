#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect backgroundTex;
uniform sampler2D normalMap;
uniform sampler2D diffuseMap;

void main()
{
	vec3 normal = vec3(texture2D(normalMap, gl_TexCoord[0].xy));
	vec2 bgCoord = gl_FragCoord.xy * normal.xy;
	vec4 bgColor = texture2DRect(backgroundTex, bgCoord);
	vec4 diffuseColor = texture2D(diffuseMap, gl_TexCoord[0].xy);
	gl_FragColor = bgColor * diffuseColor;
}
