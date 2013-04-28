#version 150

uniform sampler2D uSSAOMap;

in vec2 gPositions;

out VertexData
{
	smooth vec2 texCoord;
} 
gVertexData;

void main ()
{
	gVertexData.texCoord = gPositions;
	vec2 pos;
	pos.x = gPositions.x*2.0f - 1.0f;
	pos.y = gPositions.y*2.0f - 1.0f;
	gl_Position = vec4(pos, 0, 1);
}
