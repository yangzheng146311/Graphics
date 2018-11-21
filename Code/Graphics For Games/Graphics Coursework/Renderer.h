#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/CubeRoot.h"
#include"../../nclgl/MD5Mesh.h"
#include"../../nclgl/MD5Node.h"
#include <stdio.h> 
#include "textmesh.h"
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
	void DrawFloor(); // New !
	void DrawShadowScene(); // New !
	void DrawCombinedScene(); // New !

	void DrawFPS();
	
	string FrameRateToString(float msec);
	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

	SceneNode* root;

	Shader * textShader;
	Shader * cubeShader;
	Shader * lightShader;
	Shader * reflectShader;
	Shader * skyboxShader;
	Shader * sceneShader;
	Shader * shadowShader;
	HeightMap * heightMap;
	Mesh * quad;

	Mesh * floor;
	Light * light;
	Font*	basicFont;	//A font! a basic one...

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
	float curMsec;
	
	int dir = 1;

};