#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect backgroundTex;

varying vec3 RefractR;
varying vec3 RefractG;
varying vec3 RefractB;

void main()
{
	vec2 bgCoord;
	vec3 refractColor;
	
	bgCoord = gl_FragCoord.xy * RefractR.xy;
	refractColor.r = texture2DRect(backgroundTex, bgCoord).r;
	
	bgCoord = gl_FragCoord.xy * RefractG.xy;
	refractColor.g = texture2DRect(backgroundTex, bgCoord).g;
	
	bgCoord = gl_FragCoord.xy * RefractB.xy;
	refractColor.b = texture2DRect(backgroundTex, bgCoord).b;
	
	gl_FragColor = vec4(refractColor, 1.0);
}
