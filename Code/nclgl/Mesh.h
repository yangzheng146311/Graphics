#pragma once
#include "./../../nclgl/OGLRenderer.h"

enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER,NORMAL_BUFFER, TANGENT_BUFFER, INDEX_BUFFER, MAX_BUFFER

	//ERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, NORMAL_BUFFER,  INDEX_BUFFER, MAX_BUFFER
	
};
class Mesh {
public:
Mesh(void);
~Mesh(void);

virtual void Draw();
static Mesh * GenerateTriangle();
static Mesh* GenerateQuad();
void SetTexture(GLuint tex) { texture = tex; }
GLuint GetTexture() { return texture; }
//
void SetBumpMap(GLuint tex) { bumpTexture = tex; } 
GLuint GetBumpMap() { return bumpTexture;}


protected:
void BufferData();
void GenerateNormals();

void GenerateTangents(); 
Vector3 GenerateTangent(const Vector3 &a, const Vector3 &b, 
	const Vector3 &c, const Vector2 &ta, 
	const Vector2 &tb, const Vector2 &tc);

GLuint arrayObject;
GLuint bufferObject[MAX_BUFFER];
GLuint numVertices;
GLuint type;

Vector3 * vertices;
Vector4 * colours;
Vector3* normals;

GLuint texture;
Vector2* textureCoords;

Vector3* tangents;
GLuint bumpTexture;

GLuint numIndices; 
unsigned int* indices;
};