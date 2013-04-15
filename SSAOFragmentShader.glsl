#version 150

out vec4 gFragOutput;

in vec3 gNormal;
in vec4 gPosView;

void main ()
{
	vec3 lightDir = vec3(0.0f, 0.0f, -1.0f);
	normalize(lightDir);
	vec3 normal = gNormal;
	normalize(normal);
	
	normal.x = abs(normal.x);
	normal.y = abs(normal.y);
	normal.z = abs(normal.z);

	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 color = 0.3*vec3(1.0f, 1.0f, 0.0f) + 0.7*diff*vec3(0.0f, 1.0f, 0.0f);
	gFragOutput = vec4(normal, gl_FragCoord.z);
//	gl_FragDepth = gl_FragCoord.z; //not writing this allows for early-z
}
