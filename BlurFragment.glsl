//------------------------------------------------------------------------------
#version 150
//------------------------------------------------------------------------------
uniform sampler2D uSSAOMap;
uniform vec2 uTexelSize;
//------------------------------------------------------------------------------
in VertexData
{
	smooth vec2 texCoord;
} 
gVertexData;
//------------------------------------------------------------------------------
out vec4 oFragOut;
//------------------------------------------------------------------------------
void main ()
{
	float smoothSSAO = 0.0f;

   	for (int i = 0; i < 4; i++)
	{
    	for (int j = 0; j < 4; j++)
		{
			vec2 off = vec2(float(i)*uTexelSize.x, float(j)*uTexelSize.y);
			smoothSSAO +=texture(uSSAOMap, gVertexData.texCoord + off).x;
      	}
   	}
	
	smoothSSAO = smoothSSAO/16.0f;

	oFragOut = vec4(smoothSSAO, smoothSSAO, smoothSSAO, 1.0f);
}
//------------------------------------------------------------------------------
