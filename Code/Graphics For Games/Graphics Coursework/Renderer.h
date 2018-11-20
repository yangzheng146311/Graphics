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
	double CamOriX= 1985.0f;
	double CamOriY= 3584.0f;
	double CamOriZ= 6387.0f;
	double CamOriPitch = -37.64f;
	double CamOriYaw = 0.9f;
	double curYaw;
	
	int dir = 1;

};