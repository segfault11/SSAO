//-----------------------------------------------------------------------------
#ifndef SSAORENDERER_H
#define SSAORENDERER_H
//-----------------------------------------------------------------------------
#include "Model.h"
#include "OpenGL.h"
#include "Camera.h"
//-----------------------------------------------------------------------------
struct Vector3f
{
	float X, Y, Z;

	Vector3f ()
	: X(0.0f), Y(0.0f), Z(0.0f)
	{

	}

	Vector3f (float x, float y, float z)
	: X(x), Y(y), Z(z)
	{

	}
};
//-----------------------------------------------------------------------------
struct Camera
{
	Vector3f Position;
	Vector3f FocusPoint;
	Vector3f Up;

	float FovY, Aspect, Near, Far;
};
//-----------------------------------------------------------------------------
class SSAORenderer 
{
public:
    SSAORenderer(const Model& model, unsigned int width, unsigned int height);
    virtual ~SSAORenderer();
	
	void SetCamera (const Camera& camera);
	void SetCamera (const GL::Camera& camera);
	void Draw () const;

private:
	GLuint mModelVAO;
	GLuint mVerticesVBO;
	GLuint mNormalsVBO;
	GLuint mIndicesVBO;
	GLuint mProgram;
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
