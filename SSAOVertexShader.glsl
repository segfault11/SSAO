#version 150

uniform mat4 projMat;
uniform mat4 viewMat;

in vec3 gPositions;
in vec3 gVertNormals;

out vec3 gNormal;   // surface normal in view space
out float gLinDepth;

void main ()
{
	gNormal = gVertNormals;
	vec4 posView = viewMat*vec4(gPositions, 1.0f);
	gLinDepth = -posView.z;
	gl_Position = projMat*posView;
}
