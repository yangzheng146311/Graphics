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
	
	string FloatToString(float msec);
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
	bool camMove = true;
	float LightOriginRadius;
	float LightOriginPosZ;
	float LightOriginPosY;
	float LightOriginPosX;
	double CamOriX= 2183.0f;
	double CamOriY= 1335.0f;
	double CamOriZ= 2103.0f;
	double CamOriPitch = -24.0f;
	double CamOriYaw =49.5f;
	double curYaw;
	float curMsec;
	
	int dir = 1;

};