#version 150

uniform sampler2D uSSAOMap;

in vec2 iPositions;

out VertexData
{
	smooth vec2 texCoord;
} 
gVertexData;

void main ()
{
	gVertexData.texCoord = iPositions;
	vec2 pos;
	pos.x = iPositions.x*2.0f - 1.0f;
	pos.y = iPositions.y*2.0f - 1.0f;
	gl_Position = vec4(pos, 0, 1);
}
