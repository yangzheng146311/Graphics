#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"


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

	int dir = 1;

};