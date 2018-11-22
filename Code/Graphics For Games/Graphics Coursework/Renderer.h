#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/CubeRoot.h"
#include"../../nclgl/MD5Mesh.h"
#include"../../nclgl/MD5Node.h"
#include "ParticleEmitter.h"	//A new class!
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
	void DrawParticle();
	void DrawHell();
	void DrawScene_A();
	void DrawScene_B();
	void DrawScene_C();

	void	SetShaderParticleSize(float f);	//And a new setter
	
	string FloatToString(float msec);
	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

	SceneNode* root;
	
	Shader * hellShader;
	Shader * textShader;
	Shader * cubeShader;
	Shader * lightShader;
	Shader * reflectShader;
	Shader * skyboxShader;
	Shader * sceneShader;
	Shader * shadowShader;
	Shader * particleShader;

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

	ParticleEmitter*	emitter;	//A single particle emitter

	


	float waterRotate;
	bool lightOff = true;
	bool lightUp = true;
	bool lightFront = true;
	bool lightRight = true;
	bool camMove = false;
	bool aniWalk = true;
	bool aniIdle = false;
	bool aniAttack = false;
	bool lightOn = false;
	
	float LightOriginRadius;
	float LightOriginPosZ;
	float LightOriginPosY;
	float LightOriginPosX;
	double CamOriX= 434;
	double CamOriY= 835.0f;
	double CamOriZ= 687.0f;
	double CamOriPitch = -22.0f;
	double CamOriYaw =55.0f;
	double curYaw;
	float curMsec;

	float hellNightX=0;
	float hellNightY=110;
	float hellNightZ=0;
	float timec = 0.0f;
	
	int dir = 1;
	int curScene = 1;

	

};
