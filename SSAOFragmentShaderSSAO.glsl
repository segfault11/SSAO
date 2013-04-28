#version 150

uniform mat4 projMat;
uniform mat4 viewMat;
uniform sampler2D gkNormalDepthTex;
uniform vec3 uSamples[32];

float gRadius = 0.03f; // TODO: make generic

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

	if (depth == 0.0f)
	{
		discard;
	}

	//==========================================================================
	// reconstruct view space position from depth
	//==========================================================================
	float u = gVertexData.texCoord.x;	
	float v = gVertexData.texCoord.y;	
	
	vec3 pos;	
	pos.x = (2.0f*u - 1.0f + projMat[0][2])*depth/projMat[0][0];
	pos.y = (2.0f*v - 1.0f + projMat[1][2])*depth/projMat[1][1];
	pos.z = -depth;

	//==========================================================================
	//
	//==========================================================================
	float occ = 0.0f;
	for (int i = 0; i < 32; i++)
	{
		vec3 samplePos = pos + gRadius*uSamples[i];
		vec4 sampleOff = projMat*vec4(samplePos, 1.0f);
		sampleOff.xy /= sampleOff.w;
		sampleOff.xy = sampleOff.xy*0.5f + 0.5f;

		float sampleDepth = texture(gkNormalDepthTex, sampleOff.xy).a;
		
		// add 1.0f occ when the depth of the sample [samplePos.z] is smaller
		// than the fragment depth of the sample [sampleDepth], since 
		// [sampleDepth] is our linear depth it is positive. [samplePos.z],
		// however is negative, therefore we evaluate [samplePos.z + 
		// sampleDepth > 0.0f]
		occ += samplePos.z + sampleDepth > 0.0f ? 0.0f : 1.0f;
	}
	
	occ = 1.0 - occ/32.0f;

	gFragOutput = vec4(occ, occ, occ, 1.0f);
}
