#include "modelLoader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <glm/glm.hpp>
#include <vector>
#include <string>

vector<GLuint> BufferObjects;
vector<glm::vec3 *> vertexList;
vector<glm::vec2 *> textureList;
vector<glm::vec3 *> normalList;
vector<glm::vec3 *> faceList;
using namespace std;
modelLoader::modelLoader(const char * pModelName)
{
	ifstream modelFile;
	
	string determiner;
	modelFile.open(pModelName);
	while (modelFile >> determiner)
	{
		if (determiner == "v") {
			float holder[3];
			char trash;
			modelFile >> holder[0] >> holder[1] >> holder[2];
			vertexList.push_back(new glm::vec3(holder[0], holder[1], holder[2]));
		}
		else if (determiner == "vt") {


			float holder[2];
			char trash;
			modelFile >> holder[0] >> holder[1];
			textureList.push_back(new glm::vec2(holder[0], holder[1]));
		}
		else if (determiner == "vn") {
			float holder[3];
			char trash;
			modelFile >> holder[0] >> holder[1] >> holder[2];
			normalList.push_back(new glm::vec3(holder[0], holder[1], holder[2]));
		} 
		else if (determiner == "f") {
			// each face is made up of a set of 3 points, these points are indexed, to read them in I will create a 
			// array of floats that will be stored in tripplets
			
			for (int x = 0; x < 3; x++) {
				
				int holder[3];
				char trash;
				modelFile >> holder[0] >> trash >> holder[1] >> trash >> holder[2];
				faceList.push_back(new glm::vec3(holder[0], holder[1], holder[2]));
			}
		}
		else
		{
			
		}
		
	}
	modelFile.close();
	//we have the data for the STATIC model now let's organize it, for each face vector we will compile the indervidual data
	// and save it all in one massive amazing array of awsomeness, but is this best practice? add in other ways to store data
	//each pointer is made of 8 floats
	vector<float *> vertacies;
	if (false) { // todo add load type
		for (int x = 0; x < vertexList.size(); x++) {
			modelLoader::data.push_back(vertexList[x]->x);
			modelLoader::data.push_back(vertexList[x]->y);
			modelLoader::data.push_back(vertexList[x]->z);

		}
		for (int x = 0; x < normalList.size(); x++) {
			modelLoader::data.push_back(normalList[x]->x);
			modelLoader::data.push_back(normalList[x]->y);
			modelLoader::data.push_back(normalList[x]->z);

		}
		for (int x = 0; x < textureList.size(); x++) {
			modelLoader::data.push_back(textureList[x]->x);
			modelLoader::data.push_back(textureList[x]->y);
		}
	}
	else {
		vector<GLuint> indexes;
		vector<glm::vec3> processedObjects;
		for (int x = 0; x < modelLoader::faceList.size(); x++) {
			// check for indexing before we start
		}
		for (int x = 0; x < modelLoader::faceList.size(); x++) {
			float position	= modelLoader::faceList[x]->x;
			float texture	= modelLoader::faceList[x]->y;
			float normal	= modelLoader::faceList[x]->z;
			
			float * vertex	= new float[8];
			// check to see if this vector has already been created if it has index the shit out of it
			bool exit = false;
			for(int y = 0; y < processedObjects.size(); y++) {
				// this takes too long, we need to scan for this in advance, no point in scanning the list more than once,

				if (processedObjects[y].x == position && processedObjects[y].y == normal && processedObjects[y].z == texture) 
				{
					indexes.push_back(y);
					exit = true;
					break;
				}
			}
			if (exit) {
				continue;
			}
			// add the vertex Position
			modelLoader::data.push_back(modelLoader::vertexList[position - 1]->x);
			modelLoader::data.push_back(modelLoader::vertexList[position - 1]->y);
			modelLoader::data.push_back(modelLoader::vertexList[position - 1]->z);
			// add the normal
			modelLoader::data.push_back(modelLoader::normalList[normal - 1]->x);
			modelLoader::data.push_back(modelLoader::normalList[normal - 1]->y);
			modelLoader::data.push_back(modelLoader::normalList[normal - 1]->z);
			// add the texture coords
			modelLoader::data.push_back(modelLoader::textureList[texture - 1]->x);
			modelLoader::data.push_back(modelLoader::textureList[texture - 1]->y);
			indexes.push_back(indexes.size());
			processedObjects.push_back(*faceList[x]);
		}
	}
}
modelLoader::modelLoader(const char * pModelName, const bool pLoadFile) {
	modelLoader Model(pModelName);
	GLuint * buffers = new GLuint[2];
	glGenBuffers(2, buffers);
	BufferObjects.push_back(buffers[0]);
	BufferObjects.push_back(buffers[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, Model.getData()->size() * (sizeof(float)), &Model.getData()[0], GL_STATIC_DRAW);
	int size;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	
	
	if (Model.getData()->size() * ( 3 * sizeof(float)) != size)
	{
		throw new exception("Vertex data not loaded onto graphics card correctly");
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  Model.getFaceList()->size() * (sizeof(float) * 3), &Model.getFaceList()[0], GL_STATIC_DRAW);

	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	if (Model.getFaceList()->size() * (sizeof(float) * 3) != size)
	{
		throw new exception("Index data not loaded onto graphics card correctly");
	}
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	// idea have a library of shader locations, so that I can just directly call them and it will be set for all
	// shaders within the shader library
	GLintptr TextureCoords = (6 * sizeof(float));
	GLintptr NormalCoords = (3 * sizeof(float));
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, true,	 8 * sizeof(float), (GLvoid *)NormalCoords);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 8 * sizeof(float), (GLvoid *)TextureCoords);

}


modelLoader::~modelLoader()
{
}

