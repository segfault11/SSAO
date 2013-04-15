#include "SSAORenderer.h"
#include <iostream>
#include <cstdio>

//-----------------------------------------------------------------------------
void saveTexture1f
(
	const char* filename,
	unsigned int width,
	unsigned int height, 
	unsigned int stride, 
	unsigned int offset,
	float* data
)
{
	FILE* file = fopen(filename, "w");
	
	fprintf(file, "%s\n", "P3");
	fprintf(file, "%u %u\n", width, height);
	fprintf(file, "255\n");

	unsigned int numPixels = width*height;
	unsigned int cur = offset;
	unsigned int cur2;

	unsigned char *img = new unsigned char[3*width*height];


	for (unsigned int j = 0; j < height; j++)
	{
		for (unsigned int i = 0; i < width; i++)
		{
			cur = ((height - 1 - j)*width + i);
			cur2 = (j*width + i);
			img[3*cur + 0] = data[stride*cur2 + 0 + offset]*255;
			img[3*cur + 1] = data[stride*cur2 + 0 + offset]*255;
			img[3*cur + 2] = data[stride*cur2 + 0 + offset]*255;
		}
	}

	for (unsigned int i = 0; i < numPixels; i++)
	{
		fprintf(file, "%u %u %u\n", img[3*i + 0], img[3*i + 1], img[3*i + 2]);
	}

	delete[] img;

	fclose(file);
}
//-----------------------------------------------------------------------------
void saveTexture3f
(
	const char* filename,
	unsigned int width,
	unsigned int height, 
	unsigned int stride, 
	unsigned int offset,
	float* data
)
{
	FILE* file = fopen(filename, "w");
	
	fprintf(file, "%s\n", "P3");
	fprintf(file, "%u %u\n", width, height);
	fprintf(file, "255\n");

	unsigned int numPixels = width*height;
	unsigned int cur = offset;
	unsigned int cur2;

	unsigned char *img = new unsigned char[3*width*height];


	for (unsigned int j = 0; j < height; j++)
	{
		for (unsigned int i = 0; i < width; i++)
		{
			cur = ((height - 1 - j)*width + i);
			cur2 = (j*width + i);
			img[3*cur + 0] = data[stride*cur2 + 0 + offset]*255;
			img[3*cur + 1] = data[stride*cur2 + 1 + offset]*255;
			img[3*cur + 2] = data[stride*cur2 + 2 + offset]*255;
		}
	}

	for (unsigned int i = 0; i < numPixels; i++)
	{
		fprintf(file, "%u %u %u\n", img[3*i + 0], img[3*i + 1], img[3*i + 2]);
	}

	delete[] img;

	fclose(file);
}
//-----------------------------------------------------------------------------
SSAORenderer::SSAORenderer 
(
	const Model& model,
	unsigned int width,
	unsigned int height
)
:
	mNumVertices(model.GetNumVertices()), mNumIndices(model.GetNumFaces()*3),
	mWidth(width),
	mHeight(height)
{
	// create programm	
	mProgram = glCreateProgram();
	GL::AttachShader(mProgram, "SSAOVertexShader.glsl", GL_VERTEX_SHADER);
	GL::AttachShader(mProgram, "SSAOFragmentShader.glsl", GL_FRAGMENT_SHADER);	
	GL::BindAttribLocation(mProgram, "gPositions", 0);
	GL::BindAttribLocation(mProgram, "gNormals", 1);
	GL::BindFragDataLocation(mProgram, "gFragOutput", 0);
	GL::LinkProgram(mProgram);

	// set up vertex array object for the model
	GL::CreateBufferObject
	(
		mVerticesVBO, 
		GL_ARRAY_BUFFER,
		model.GetNumVertices()*3*sizeof(float),
		model.GetVertices(),
		GL_STATIC_DRAW
	);
	GL::CreateBufferObject
	(
		mNormalsVBO, 
		GL_ARRAY_BUFFER,
		model.GetNumVertices()*3*sizeof(float),
		model.GetVertexNormals(),
		GL_STATIC_DRAW
	);	
	GL::CreateBufferObject
	(
		mIndicesVBO, 
		GL_ELEMENT_ARRAY_BUFFER,
		model.GetNumFaces()*3*sizeof(unsigned int),
		model.GetFaces(),
		GL_STATIC_DRAW
	);

	glGenVertexArrays(1, &mModelVAO);
	glBindVertexArray(mModelVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, mNormalsVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	
	//
	// set up render targets for color, pos, normal buffer
	//
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);

	// add a RBO to the FBO, because we want depth test
	glGenRenderbuffers(1, &mRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWidth, mHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
		GL_RENDERBUFFER, mRBO);

	// create textures
	glGenTextures(3, mTex);

	// first texture is for depth and the normals
	glBindTexture(GL_TEXTURE_2D, mTex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mWidth, mHeight, 0, GL_RGBA, 
		GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
		GL_TEXTURE_2D, mTex[0], 0);
	
	// tell opengl we want to render into multiple targets
	GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, buffers);	
	

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "FR N OK" << std::endl;
	}
	else
	{
		std::cout << "FROK" << std::endl;
	}
		
	//unbind framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
};
//-----------------------------------------------------------------------------
SSAORenderer::~SSAORenderer ()
{
	glDeleteProgram(mProgram);	
	glDeleteBuffers(1, &mVerticesVBO);
	glDeleteBuffers(1, &mNormalsVBO);
	glDeleteBuffers(1, &mIndicesVBO);
	glDeleteVertexArrays(1, &mModelVAO);
}
//-----------------------------------------------------------------------------
void SSAORenderer::Draw () const
{
//	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, mTex[0]);
	glUseProgram(mProgram);
	glBindVertexArray(mModelVAO);
	glDrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0);

//	glBindTexture(GL_TEXTURE_2D, mTex[0]);
//	float *data = new float[mWidth*mHeight*4];
//	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data);
//	saveTexture1f("t.ppm", mWidth, mHeight, 4, 3, data);

//	exit(0);
}
//-----------------------------------------------------------------------------
void SSAORenderer::SetCamera (const Camera& camera)
{
	GLfloat projMat[16];
	GL::Perspective
	(
		camera.FovY, 
		camera.Aspect, 
		camera.Near, 
		camera.Far, 
		projMat
	);
	GLfloat viewMat[16];
	GL::LookAt
	(
		camera.Position.X,
		camera.Position.Y,
		camera.Position.Z,
		camera.FocusPoint.X,
		camera.FocusPoint.Y,
		camera.FocusPoint.Z,
		camera.Up.X,
		camera.Up.Y,
		camera.Up.Z,
		viewMat
	);

	GLint projMatLoc = glGetUniformLocation(mProgram, "projMat");
	GLint viewMatLoc = glGetUniformLocation(mProgram, "viewMat");
	
	glUseProgram(mProgram);
	glUniformMatrix4fv(projMatLoc, 1, false, projMat);
	glUniformMatrix4fv(viewMatLoc, 1, false, viewMat);
}
//-----------------------------------------------------------------------------
void SSAORenderer::SetCamera (const GL::Camera& camera)
{
	GLfloat projMat[16];
	GL::Camera::ComputeProjectionMatrix(projMat, camera);
	GLfloat viewMat[16];
	GL::Camera::ComputeViewMatrix(viewMat, camera);

	
	GLint projMatLoc = glGetUniformLocation(mProgram, "projMat");
	GLint viewMatLoc = glGetUniformLocation(mProgram, "viewMat");
	
	glUseProgram(mProgram);
	glUniformMatrix4fv(projMatLoc, 1, false, projMat);
	glUniformMatrix4fv(viewMatLoc, 1, false, viewMat);
}
//-----------------------------------------------------------------------------
