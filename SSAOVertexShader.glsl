//------------------------------------------------------------------------------
#version 150
//------------------------------------------------------------------------------
uniform mat4 projMat;
uniform mat4 viewMat;
//------------------------------------------------------------------------------
in vec3 gPositions;
in vec3 gVertNormals;
//------------------------------------------------------------------------------
out vec3 gNormal;   	// surface normal in view space
out vec3 gPosView; 		// surface position in view space
out float gLinDepth; 	// linear depth
//------------------------------------------------------------------------------
void main ()
{
	gNormal = (transpose(inverse(viewMat))*vec4(gVertNormals, 1.0f)).xyz;
	vec4 posView = viewMat*vec4(gPositions, 1.0f);
	gPosView = vec3(posView.x, posView.y, posView.z);
	gLinDepth = -posView.z;
	gl_Position = projMat*posView;
}
//------------------------------------------------------------------------------
