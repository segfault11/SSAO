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
	GLuint mProgram[2];
	unsigned int mNumVertices;
	unsigned int mNumIndices;
	unsigned int mWidth;
	unsigned int mHeight;
	GLuint mFBO;
	GLuint mRBO; 		// for depth test
	GLuint mTex[3];
};
//-----------------------------------------------------------------------------
#endif /* end of include guard: SSAORENDERER_H */
//-----------------------------------------------------------------------------
