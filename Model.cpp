#include "Model.h"
#include <fstream>
#include <stdexcept>
#include <iostream>

//-----------------------------------------------------------------------------
void reorder 
(
	float* vertexNormals, 
	const float* tmpNormals, 
	const unsigned int* faces,
	const unsigned int* tmpFaceNormals,
	unsigned int i
)
{
	unsigned int idx = faces[i];
	unsigned int idx2 = tmpFaceNormals[i];
	
	vertexNormals[3*idx + 0] = tmpNormals[3*idx2 + 0];
	vertexNormals[3*idx + 1] = tmpNormals[3*idx2 + 1];
	vertexNormals[3*idx + 2] = tmpNormals[3*idx2 + 2];
}
//-----------------------------------------------------------------------------
void count (std::ifstream& istream, unsigned int& numVertices, 
	unsigned int& numFaces)
{
	std::string line;

	while (!istream.eof()) 
	{
		std::getline(istream, line);
	    
		if (line.c_str()[0] == 'v' && line.c_str()[1] == ' ')
		{
			numVertices++;
		}

		if (line.c_str()[0] == 'f' && line.c_str()[1] == ' ')
		{
			numFaces++;
		}
	

	}
}
//-----------------------------------------------------------------------------
void fill 
(
	std::ifstream& istream, 
	float* vertices, 
	float* vertexNormals,
	unsigned int* faces, 
	const unsigned int numVertices, 
	const unsigned int numFaces
)
{
	unsigned int vertexCount = 0;
	unsigned int faceCount = 0;
	unsigned int normalCount = 0;
	int check;
	std::string line;
	
	float* tmpNormals = new float[3*numVertices];
	unsigned int* tmpFaceNormals = new unsigned int[3*numFaces];
	
	while (!istream.eof()) 
	{
		std::getline(istream, line);
		
		// get all vertices
		if (line.c_str()[0] == 'v' && line.c_str()[1] == ' ')
		{
			check = sscanf
			(
				line.c_str(), 
				"v %f %f %f", 
				&vertices[vertexCount + 0],
				&vertices[vertexCount + 1],
				&vertices[vertexCount + 2]
			);

			if (check != 3) 
			{
		    	throw std::runtime_error("invalid file.");
			}

			vertexCount += 3;
		}
		
		// get all vertex normals
		if (line.c_str()[0] == 'v' && line.c_str()[1] == 'n')
		{
			check = sscanf
			(
				line.c_str(), 
				"vn %f %f %f", 
				&tmpNormals[normalCount + 0],
				&tmpNormals[normalCount + 1],
				&tmpNormals[normalCount + 2]
			);
			
			if (check != 3) 
			{
		    	throw std::runtime_error("invalid file.");
			}

			normalCount += 3;
		}
		
		// collect all faces
		if (line.c_str()[0] == 'f' && line.c_str()[1] == ' ')
		{
			check = sscanf
			(
				line.c_str(), 
				"f %d//%d %d//%d %d//%d", 
				&faces[faceCount + 0],
				&tmpFaceNormals[faceCount + 0],
				&faces[faceCount + 1],
				&tmpFaceNormals[faceCount + 1],
				&faces[faceCount + 2],
				&tmpFaceNormals[faceCount + 2]
			);

			if (check != 6) 
			{
		    	std::cout << line << std::endl;
				throw std::runtime_error("invalid file.");
			}
		
			faceCount += 3;
		}
		
	}
	
	// substract 1 from each index!!
	for (unsigned int i = 0; i < 3*numFaces; i++) 
	{
	    faces[i] -= 1;
		tmpFaceNormals[i] -= 1;
	}

	// reorder normals
	for (unsigned int i = 0; i < 3*numFaces; i++) 
	{
	    reorder(vertexNormals, tmpNormals, faces, tmpFaceNormals, i);
	}

	delete[] tmpNormals;
	delete[] tmpFaceNormals;
}
//-----------------------------------------------------------------------------
Model::Model (const std::string& filename)
: mVertices(NULL), mVerticeNormals(NULL), mFaces(NULL), mNumVertices(0), 
mNumFaces(0)
{
	std::ifstream file(filename.c_str());
	
	if (!file.is_open()) 
	{
		throw std::runtime_error("Could not open file.");    
	}

	std::string line;
	
	// count vertices an faces
	count(file, mNumVertices, mNumFaces);

	// allocate memory
	mVertices = new float[3*mNumVertices];
	mVerticeNormals = new float[3*mNumVertices];
	mFaces = new unsigned int[3*mNumFaces];	

	// reset file pointer
	file.clear();	
	file.seekg(std::ios::beg);

	// fill arrays
	fill(file, mVertices, mVerticeNormals, mFaces, mNumVertices, mNumFaces);	

	file.close();
}
//-----------------------------------------------------------------------------
Model::~Model ()
{
	delete[] mVertices;
	delete[] mVerticeNormals;
	delete[] mFaces;
}
//-----------------------------------------------------------------------------
const float* Model::GetVertices () const
{
	return mVertices;
}
//-----------------------------------------------------------------------------
const float* Model::GetVertexNormals () const
{
	return mVerticeNormals;
}
//-----------------------------------------------------------------------------
const unsigned int* Model::GetFaces () const
{
	return mFaces;
}
//-----------------------------------------------------------------------------
unsigned int Model::GetNumVertices () const
{
	return mNumVertices;
}
//-----------------------------------------------------------------------------
unsigned int Model::GetNumFaces () const
{
	return mNumFaces;
}
//-----------------------------------------------------------------------------
void Model::Dump () const
{
	printf(">>> Vertices\n");
	for (unsigned int i = 0; i < mNumVertices; i++)
	{
		printf
		(
			"v %f %f %f\n",
			mVertices[3*i + 0], 
			mVertices[3*i + 1], 
			mVertices[3*i + 2]
		);
	}
	
	printf(">>> Vertex Normals\n");
	for (unsigned int i = 0; i < mNumVertices; i++)
	{
		printf
		(
			"vn %f %f %f\n",
			mVerticeNormals[3*i + 0], 
			mVerticeNormals[3*i + 1], 
			mVerticeNormals[3*i + 2]
		);
	}

	printf(">>> Indices\n");
	for (unsigned int i = 0; i < mNumFaces; i++)
	{
		printf
		(
			"f %d %d %d\n",
			mFaces[3*i + 0],
			mFaces[3*i + 1],
			mFaces[3*i + 2]
		);
	}

}
//-----------------------------------------------------------------------------

