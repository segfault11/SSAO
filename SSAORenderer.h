//-----------------------------------------------------------------------------
#ifndef SSAORENDERER_H
#define SSAORENDERER_H
//-----------------------------------------------------------------------------
#include "Model.h"
#include "OpenGL.h"
#include "Camera.h"
//-----------------------------------------------------------------------------
class SSAORenderer 
{
public:
    SSAORenderer(const Model& model, unsigned int width, unsigned int height);
    virtual ~SSAORenderer();
	
	void SetCamera (const GL::Camera& camera);
	void Draw () const;

private:
	// fills textures with geometry information
	void fillTextures () const;

	GLuint mModelVAO;
	GLuint mQuadVAO; 	 // Vertex array obj. that stores a quad
	GLuint mVerticesVBO;
	GLuint mNormalsVBO;
	GLuint mIndicesVBO;
	GLuint mQuadVBO;     // Vertex buffer obj. that stores a quad
	GLuint mProgram[3];
	unsigned int mNumVertices;
	unsigned int mNumIndices;
	unsigned int mWidth;
	unsigned int mHeight;
	GLuint mFBO;
	GLuint mRBO; 		// for depth test
	GLuint mTex[4];
	GLuint mSSAOFBO; 	// FBO for rendering the SSAO vals into a texture
	GL::Vector3f mSamples[64];
};
//-----------------------------------------------------------------------------
#endif /* end of include guard: SSAORENDERER_H */
//-----------------------------------------------------------------------------
