//------------------------------------------------------------------------------
#include "SSAORenderer.h"
#include <iostream>
#include <cstdio>
#include "Random.h"
//------------------------------------------------------------------------------
static float gsQuadCoords[] = 
{
	0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
};
//------------------------------------------------------------------------------
static Random gsRandom;
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
	FILE* fileDepth = fopen("depthmap.ppm", "w");
	
	fprintf(file, "%s\n", "P3");
	fprintf(file, "%u %u\n", width, height);
	fprintf(file, "255\n");

	fprintf(fileDepth, "%s\n", "P3");
	fprintf(fileDepth, "%u %u\n", width, height);
	fprintf(fileDepth, "255\n");

	unsigned int numPixels = width*height;
	unsigned int cur = offset;
	unsigned int cur2;
	unsigned char* img = new unsigned char[3*width*height];
	unsigned char* imgDepth = new unsigned char[3*width*height];
	float max = 0.0f;
	float min = 1.0f;

	for (unsigned int j = 0; j < height; j++)
	{
		for (unsigned int i = 0; i < width; i++)
		{
			cur2 = (j*width + i);
			float depth = (data[stride*cur2 + 3 + offset]);

			if (depth > max && depth != 1.0f) 
			{
			    max = depth;
			}

			if (depth < min)
			{
				min = depth;
			}
		}
	}

	std::cout << "min " << min << std::endl;
	std::cout << "max " << max << std::endl;

	for (unsigned int j = 0; j < height; j++)
	{
		for (unsigned int i = 0; i < width; i++)
		{
			cur = ((height - 1 - j)*width + i);
			cur2 = (j*width + i);
			img[3*cur + 0] = data[stride*cur2 + 0 + offset]*255;
			img[3*cur + 1] = data[stride*cur2 + 1 + offset]*255;
			img[3*cur + 2] = data[stride*cur2 + 2 + offset]*255;

			float depth = data[stride*cur2 + 3 + offset];
			
			if (depth != 1.0f) 
			{
				depth = (depth - min)/(max - min);    
			}


			imgDepth[3*cur + 0] = depth*255;
			imgDepth[3*cur + 1] = depth*255;
			imgDepth[3*cur + 2] = depth*255;
		}
	}

	for (unsigned int i = 0; i < numPixels; i++)
	{
		fprintf(file, "%u %u %u\n", img[3*i + 0], img[3*i + 1], img[3*i + 2]);
	}

	for (unsigned int i = 0; i < numPixels; i++)
	{
		fprintf
		(
			fileDepth, 
			"%u %u %u\n", 
			imgDepth[3*i + 0], 
			imgDepth[3*i + 1], 
			imgDepth[3*i + 2]
		);
	}


	delete[] img;
	delete[] imgDepth;

	fclose(file);
}
//------------------------------------------------------------------------------
inline float lerp (float a, float b, float x)
{
	return a + x*(b - a);
}
//------------------------------------------------------------------------------
inline void createKernel(GL::Vector3f* kernel, unsigned int size)
{
	for (unsigned int i = 0; i < size; i++)
	{
		kernel[i].X() = gsRandom.GetRandomRanged(-1.0f, 1.0f);
		kernel[i].Y() = gsRandom.GetRandomRanged(-1.0f, 1.0f);
		kernel[i].Z() = gsRandom.GetRandomRanged(0.0f, 1.0f);
		kernel[i].Normalize();
		float scale = static_cast<float>(i)/static_cast<float>(size);
   		scale = lerp(0.1f, 1.0f, scale*scale);
   		kernel[i] *= scale;
	}
}
//-----------------------------------------------------------------------------
inline void createNoiseTexture (GLuint& handle, unsigned int size)
{
	// creates a size x size rgb floating point texture, and stores random noise
	// axis in the xy plane. these axis are later used in the ssao fragment 
	// shader together with the fragments normal to create a random space
	// for kernel samples, to introduce noise to the ssao map and to avoid
	// banding effects

	// create noise axis
	unsigned int numSamples = size*size;
	GL::Vector3f* samples = new GL::Vector3f[numSamples];
	
	for (unsigned int i = 0; i < numSamples; i++)
	{
		samples[i].X() = gsRandom.GetRandomRanged(-1.0f, 1.0f);
		samples[i].Y() = gsRandom.GetRandomRanged(-1.0f, 1.0f);
		samples[i].Z() = 0.0f; 
		samples[i].Normalize();
	}
	
	// create and init texture
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, size, 0, GL_RGB, 
		GL_FLOAT, reinterpret_cast<float*>(samples));

	delete[] samples;
}
//------------------------------------------------------------------------------
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
	//==========================================================================
	// create programm	
	//==========================================================================
	mProgram[0] = glCreateProgram();
	GL::AttachShader(mProgram[0], "SSAOVertexShader.glsl", GL_VERTEX_SHADER);
	GL::AttachShader
	(
		mProgram[0], 
		"SSAOFragmentShader.glsl", 
		GL_FRAGMENT_SHADER
	);	
	GL::BindAttribLocation(mProgram[0], "gPositions", 0);
	GL::BindAttribLocation(mProgram[0], "gNormals", 1);
	GL::BindFragDataLocation(mProgram[0], "gFragOutput", 0);
	GL::LinkProgram(mProgram[0]);

	//==========================================================================
	// set up vertex array object for the model
	//==========================================================================
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

	//==========================================================================
	// set up render targets for the 1st pass:
	// color, normal buffer, depth buffer
	//==========================================================================
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);

	// add a RBO to the FBO, because we want depth test
	glGenRenderbuffers(1, &mRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWidth, mHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
		GL_RENDERBUFFER, mRBO);

	// create textures
	glGenTextures(1, &mTex[0]);

	// first texture is for depth and the normals
	glBindTexture(GL_TEXTURE_2D, mTex[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

	//==========================================================================
	// 	Set up second program for the SSAO Values
	//==========================================================================
	mProgram[1] = glCreateProgram();
	GL::AttachShader
	(
		mProgram[1], 
		"SSAOVertexShaderSSAO.glsl", 
		GL_VERTEX_SHADER
	);
	GL::AttachShader
	(
		mProgram[1], 
		"SSAOFragmentShaderSSAO.glsl", 
		GL_FRAGMENT_SHADER
	);	
	GL::BindAttribLocation(mProgram[1], "gPositions", 0);
	GL::BindFragDataLocation(mProgram[1], "gFragOutput", 0);
	GL::LinkProgram(mProgram[1]);

	// set texture units
	glUseProgram(mProgram[1]);
	GLint loc = glGetUniformLocation(mProgram[1], "gkNormalDepthTex");
	glUniform1i(loc, 0);

	// create a 4x4 noise tex and tell our ssao shader that we plan to bind it
	// to the 1st texture unit
	createNoiseTexture(mTex[1], 4);
	loc = glGetUniformLocation(mProgram[1], "uNoiseTex");
	glUniform1i(loc, 1);
	
	// create the sample kernel and pass it to the shader	
	createKernel(mSamples, 32);
	loc = glGetUniformLocation(mProgram[1], "uSamples");
	glUniform3fv(loc, 32, reinterpret_cast<float*>(mSamples));

	// let the shader know the sample size of the kernel and the radius of the
	// sample hemisphere
	loc = glGetUniformLocation(mProgram[1], "uSampleSize");
	glUniform1i(loc, 32);
	loc = glGetUniformLocation(mProgram[1], "uSampleRadius");
	glUniform1f(loc, 0.01f);

	// let the shader know the screen width and height and the size of the 
	// noise tile
	loc = glGetUniformLocation(mProgram[1], "uWidth");
	glUniform1i(loc, 1280);
	loc = glGetUniformLocation(mProgram[1], "uHeight");
	glUniform1i(loc, 800);
	loc = glGetUniformLocation(mProgram[1], "uNoiseSize");
	glUniform1i(loc, 4);
	
	//==========================================================================
	// 	Set up blurring program for the smooth SSAO Values
	//==========================================================================
	mProgram[2] = glCreateProgram();
	GL::AttachShader
	(
		mProgram[2], 
		"BlurVertex.glsl", 
		GL_VERTEX_SHADER
	);
	GL::AttachShader
	(
		mProgram[2], 
		"BlurFragment.glsl", 
		GL_FRAGMENT_SHADER
	);	
	GL::BindAttribLocation(mProgram[2], "iPositions", 0);
	GL::BindFragDataLocation(mProgram[2], "oFragOut", 0);
	GL::LinkProgram(mProgram[2]);

	// set texture units
	glUseProgram(mProgram[2]);
	loc = glGetUniformLocation(mProgram[2], "uSSAOMap");
	glUniform1i(loc, 0);
	
	// tell the shader the size of a texel
	GLfloat texelSize[2];
	texelSize[0] = 1.0f/1280.0f; // TODO: make generic
	texelSize[1] = 1.0f/800.0f; // TODO: make generic
	loc = glGetUniformLocation(mProgram[2], "uTexelSize");
	glUniform2f(loc, texelSize[0], texelSize[1]);
	
	//==========================================================================
	// set up render targets for the 2nd pass:
	//==========================================================================
	glGenFramebuffers(1, &mSSAOFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOFBO);

	// add a RBO to the FBO, because we want depth test
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
		GL_RENDERBUFFER, mRBO);

	// create textures
	glGenTextures(1, &mTex[2]);
	glBindTexture(GL_TEXTURE_2D, mTex[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mWidth, mHeight, 0, GL_RED, 
		GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
		GL_TEXTURE_2D, mTex[2], 0);

	// tell opengl we want to render into multiple targets
	GLenum buffers2[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, buffers2);	
	
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

	//==========================================================================
	// create VAO for the quad
	//==========================================================================
	GL::CreateBufferObject
	(
		mQuadVBO, 
		GL_ARRAY_BUFFER,
		6*2*sizeof(float),
		gsQuadCoords,
		GL_STATIC_DRAW
	);

	glGenVertexArrays(1, &mQuadVAO);
	glBindVertexArray(mQuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
};
//-----------------------------------------------------------------------------
SSAORenderer::~SSAORenderer ()
{
	glDeleteProgram(mProgram[0]);	
	glDeleteBuffers(1, &mVerticesVBO);
	glDeleteBuffers(1, &mNormalsVBO);
	glDeleteBuffers(1, &mIndicesVBO);
	glDeleteVertexArrays(1, &mModelVAO);
}
//-----------------------------------------------------------------------------
void SSAORenderer::Draw () const
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(mProgram[0]);
	glBindVertexArray(mModelVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTex[0]);
	glDrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0);
	glFlush();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, mSSAOFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTex[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mTex[1]);
	glUseProgram(mProgram[1]);
	glBindVertexArray(mQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glFlush();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTex[2]);
	glUseProgram(mProgram[2]);
	glBindVertexArray(mQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 12);
}
//-----------------------------------------------------------------------------
void SSAORenderer::SetCamera (const GL::Camera& camera)
{
	GLfloat projMat[16];
	GL::Camera::ComputeProjectionMatrix(projMat, camera);
	GLfloat viewMat[16];
	GL::Camera::ComputeViewMatrix(viewMat, camera);

	GLint projMatLoc = glGetUniformLocation(mProgram[0], "projMat");
	GLint viewMatLoc = glGetUniformLocation(mProgram[0], "viewMat");

	glUseProgram(mProgram[0]);
	glUniformMatrix4fv(projMatLoc, 1, false, projMat);
	glUniformMatrix4fv(viewMatLoc, 1, false, viewMat);

    projMatLoc = glGetUniformLocation(mProgram[1], "projMat");

	glUseProgram(mProgram[1]);
	glUniformMatrix4fv(projMatLoc, 1, false, projMat);
}
//-----------------------------------------------------------------------------
