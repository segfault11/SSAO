#version 150

uniform mat4 projMat;
uniform mat4 viewMat;
uniform sampler2D gkNormalDepthTex;

in VertexData
{
	smooth vec2 texCoord;
} 
gVertexData;


out vec4 gFragOutput;

void main ()
{
	vec4 color = texture(gkNormalDepthTex, gVertexData.texCoord);

	float depth = color.a;

	if (depth == 1.0f)
	{
		discard;
	}

	float u = gVertexData.texCoord.x;	
	float v = gVertexData.texCoord.y;	
	
	vec3 pos;	
	pos.x = (2.0f*u - 1.0f + projMat[0][2])*depth/projMat[0][0];
	pos.y = (2.0f*v - 1.0f + projMat[1][2])*depth/projMat[1][1];
	pos.z = -depth;

	gFragOutput = vec4(pos, 1.0f);
}
