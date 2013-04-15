#version 150

uniform mat4 projMat;
uniform mat4 viewMat;

in vec3 gPositions;
in vec3 gVertNormals;

out vec3 gNormal;   // surface normal in view space
out vec4 gPosView;  // surface position in view space

void main ()
{
	gNormal = gVertNormals;
	gPosView = viewMat*vec4(gPositions, 1.0f);
	gl_Position = projMat*gPosView;
}
