#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/OBJmesh.h" 
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/CubeRoot.h"
#include"../../nclgl/MD5Mesh.h"
#include"../../nclgl/MD5Node.h"
#include "ParticleEmitter.h"	//A new class!
#include <stdio.h> 
#include "textmesh.h"
#define LIGHTNUM 8
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
	void FillBuffers(); //G-Buffer Fill Render Pass 
	void DrawPointLights(); //Lighting Render Pass 
	void CombineBuffers(); //Combination Render Pass 
	void GenerateScreenTexture(GLuint &into, bool depth = false);//Make a new texture... 
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
	Shader* _sceneShader; //Shader to fill our GBuffers 
	Shader* pointlightShader; //Shader to calculate lighting 
	Shader* combineShader; //shader to stick it all together 
	
	HeightMap * heightMap;
	Mesh* _heightMap; //Terrain! 
	Mesh * quad;
	Mesh * _quad;

	Mesh * floor;
	Light * light;
	Light* pointLights; //Array of lighting data 
	Font*	basicFont;	//A font! a basic one...
	OBJMesh* sphere; //Light volume 
	MD5FileData * hellData;
	MD5Node * hellNode;
	Camera * camera;
	GLuint cubeMap;
	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint bufferFBO; //FBO for our G-Buffer pass 
	GLuint bufferColourTex; //Albedo goes here 
	GLuint bufferNormalTex; //Normals go here 
	GLuint bufferDepthTex; //Depth goes here 
	GLuint pointLightFBO; //FBO for our lighting pass 
	GLuint lightEmissiveTex; //Store emissive lighting 
	GLuint lightSpecularTex; //Store specular lighting 

	ParticleEmitter*	emitter;	//A single particle emitter

	Vector3 cam_S1_OriPos=Vector3(-488,928,1623);
	double S1_CamOriPitch = -19;
	double S1_CamOriYalw = 4;
	Vector3 cam_S2_OriPos= Vector3(-859, 425, 1572);
	double S2_CamOriPitch = 9;
	double S2_CamOriYalw = 328;
	Vector3 cam_S3_OriPos = Vector3(-2087,2843, 5308);
	double S3_CamOriPitch = -25;
	double S3_CamOriYalw = 309;



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
	bool timeStop = false;
	bool toggleCam = true;
	float toggleCD = 0.0f;
	float toggleCamCD = 0.0f;
	float LightOriginRadius;
	float LightOriginPosZ;
	float LightOriginPosY;
	float LightOriginPosX;
	
	double curYaw;
	double oriYaw;
	float curMsec;

	float hellNightX=0;
	float hellNightY=110;
	float hellNightZ=0;
	float timec = 0.0f;
	float stopCD = 0.0f;
	float rotation; //How much to increase rotation by
	int dir = 1;
	int curScene = 1;

	

};
