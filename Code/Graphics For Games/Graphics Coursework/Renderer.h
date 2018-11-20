#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/CubeRoot.h"
#include"../../nclgl/MD5Mesh.h"
#include"../../nclgl/MD5Node.h"
#define SHADOWSIZE 2048//New!
class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:

	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();



	void DrawNode(SceneNode*n);
	void DrawCube();


	void DrawMesh(); // New !
	void DrawShadowScene(); // New !
	void DrawCombinedScene(); // New !

	SceneNode* root;

	Shader * cubeShader;
	Shader * lightShader;
	Shader * reflectShader;
	Shader * skyboxShader;
	Shader * sceneShader;
	Shader * shadowShader;
	HeightMap * heightMap;
	Mesh * quad;
	Light * light;
	
	MD5FileData * hellData;
	MD5Node * hellNode;
	Camera * camera;
	GLuint cubeMap;
	GLuint shadowTex;
	GLuint shadowFBO;
	float waterRotate;
	bool lightOff = true;
	bool lightUp = true;
	bool lightFront = true;
	bool lightRight = true;
	float LightOriginRadius;
	float LightOriginPosZ;
	float LightOriginPosY;
	float LightOriginPosX;
	double CamOriX= 812.0f;
	double CamOriY= 582.0f;
	double CamOriZ= 363.0f;
	double CamOriPitch = -13.4f;
	double CamOriYaw =67.0f;
	double curYaw;
	
	int dir = 1;

};