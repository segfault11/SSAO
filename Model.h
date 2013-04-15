#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <GL/GLEW.h>

class Model 
{
public:
    Model (const std::string& filename);
    virtual ~Model();
	
	const float* GetVertices () const;
	const float* GetVertexNormals () const;
	const unsigned int* GetFaces () const;

	unsigned int GetNumVertices () const;
	unsigned int GetNumFaces () const;

	void Dump () const;

private:
    float* mVertices;
	float* mVerticeNormals;
	unsigned int* mFaces;
	
	unsigned int mNumVertices;
	unsigned int mNumFaces;	
};

#endif // end of includeguard Model.h
