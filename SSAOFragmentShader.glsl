//------------------------------------------------------------------------------
#version 150
//------------------------------------------------------------------------------
out vec4 oNormalDepth;
out vec4 oScene;
//------------------------------------------------------------------------------
in vec3 gNormal;
in float gLinDepth;
in vec3 gPosView;
//------------------------------------------------------------------------------
void main ()
{
	vec3 lightDir = vec3(0.0f, 0.0f, 1.0f);
	normalize(lightDir);
	vec3 normal = gNormal;
	normalize(normal);
	vec3 posView = gPosView;
	normalize(posView);
	vec3 refl = 2.0f*dot(lightDir, normal)*normal - lightDir;
	normalize(refl);
	float diff = max(dot(lightDir, normal), 0.0f);
	float spec = max(dot(-posView, refl), 0.0f);
	vec3 color = 0.5*vec3(1.0f, 1.0f, 1.0f) + vec3(1.0f, 1.0f, 1.0f)*
		(0.5*diff + 0.0*spec*spec*spec*spec*spec*spec*spec);
	oNormalDepth = vec4(normal, gLinDepth);
	oScene = vec4(color, 1.0f);
}
//------------------------------------------------------------------------------
