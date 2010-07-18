const float EtaR = 0.65; 	//original was 0.65
const float EtaG = 0.67;	//original was 0.67
const float EtaB = 0.69;	//original was 0.69

varying vec3 RefractR;
varying vec3 RefractG;
varying vec3 RefractB;

void main()
{
	vec4 eyePos = gl_ModelViewMatrix * gl_Vertex;
	vec3 eyePos3 = eyePos.xyz / eyePos.w;
	
	vec3 i = normalize(eyePos3);
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);

	RefractR = refract(i, n, EtaR);
	RefractG = refract(i, n, EtaG);
	RefractB = refract(i, n, EtaB);

	gl_Position = ftransform();
}