#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/CubeRoot.h"

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
	SceneNode* root;

	Shader * cubeShader;
	Shader * lightShader;
	Shader * reflectShader;
	Shader * skyboxShader;
	HeightMap * heightMap;
	Mesh * quad;
	Light * light;
	
	Camera * camera;
	GLuint cubeMap;
	float waterRotate;
	bool lightOff = true;
	bool lightUp = true;
	bool lightFront = true;
	bool lightRight = true;
	float LightOriginRadius;
	float LightOriginPosZ;
	float LightOriginPosY;
	float LightOriginPosX;
	double CamOriX= 1088.0f;
	double CamOriY= 666.0f;
	double CamOriZ= -910.0f;
	double CamOriPitch = -12.0f;
	double CamOriYaw =141.0f;
	double curYaw;
	
	int dir = 1;

};