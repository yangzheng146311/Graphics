#include "Renderer.h"
#include"time.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	camera = new Camera();
	heightMap = new HeightMap("../../Textures/lalala.raw");
	quad = Mesh::GenerateQuad();

	camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 1000.0f, RAW_WIDTH * HEIGHTMAP_X));

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)), Vector4(0.9f, 0.9f, 0.9f, 1), (RAW_WIDTH * HEIGHTMAP_X) / 0.2f);
	
	LightOriginRadius = light->GetRadius();
	LightOriginPosZ = light->GetPosition().z;
	LightOriginPosY = light->GetPosition().y;
	LightOriginPosX = light->GetPosition().x;



	reflectShader = new Shader("../../Shaders/PerPixelVertex.glsl", "../../Shaders/reflectFragment.glsl");
	skyboxShader = new Shader("../../Shaders/skyboxVertex.glsl", "../../Shaders/skyboxFragment.glsl");
	lightShader = new Shader("../../Shaders/PerPixelVertex.glsl", "../../Shaders/PerPixelFragment.glsl");

	if (!reflectShader->LinkProgram() || !lightShader->LinkProgram() || !skyboxShader->LinkProgram()) {
		return;

	}
	quad->SetTexture(SOIL_load_OGL_texture("../../Textures/water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetTexture(SOIL_load_OGL_texture("../../Textures/Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture("../../Textures/Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));



	cubeMap = SOIL_load_OGL_cubemap("../../Textures/MyTextures/purplenebula_lf.tga", "../../Textures/MyTextures/purplenebula_rt.tga", "../../Textures/MyTextures/purplenebula_up.tga", "../../Textures/MyTextures/purplenebula_dn.tga",
		"../../Textures/MyTextures/purplenebula_bk.tga", "../../Textures/MyTextures/purplenebula_ft.tga", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);


	if (!cubeMap || !quad->GetTexture() || !heightMap->GetTexture() || !heightMap->GetBumpMap()) {
		return;
	}

	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);

	init = true;
	waterRotate = 0.0f;
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete light;

	currentShader = 0;

}
void Renderer::UpdateScene(float msec) {


	////Light Radius Change
	//{
	//	float r = light->GetRadius();
	//	if (r > LightOriginRadius)
	//	{

	//		lightOff = true;
	//	}

	//	if (r < 0.0f)
	//	{

	//		lightOff = false;
	//	}


	//	if (lightOff == true)
	//	{
	//		r -= 50;
	//	}
	//	else
	//	{
	//		r += 50;
	//	}

	//	light->SetRadius(r);
	//}

	////Light Color Change
	{
		Vector4 colour = light->GetColour();

		colour.x -= 0.001f;
		if (colour.x < 0) colour.x = 1.0f;
		colour.y -= 0.004f;
		if (colour.y < 0) colour.y = 1.0f;
		colour.z -= 0.007f;
		if (colour.z < 0) colour.z = 1.0f;
		light->SetColour(colour);
	}

	//Light Position Change
	{
		Vector3 pos = light->GetPosition();
		
		if (pos.x > LightOriginPosX + 2000.0f)
		{

			lightRight = false;
		}

		if (pos.x < LightOriginPosX-2000.0f)
		{

			lightRight = true;
		}

		if (pos.y > LightOriginPosY + 2000.f)
		{

			lightUp = false;
		}

		if (pos.y < LightOriginPosY-2000.f)
		{

			lightUp = true;;
		}
		if (pos.z > LightOriginPosZ + 2000.0f)
		{

			lightFront = false;
		}
		if (pos.z < LightOriginPosZ-2000.0f)
		{

			lightFront = true;
		}
		if (lightRight == true) pos.x += 10.f;
		else pos.x -= 10.0f;
		/*if (lightUp == true) pos.y += 10.f;
		else pos.y -= 10.0f;*/
		if (lightFront == true) pos.z += 10.f;
		else pos.z -= 10.0f;


		light->SetPosition(pos);
		
		
	}


	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += msec / 1000.0f;

}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	SwapBuffers();

}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);
	UpdateShaderMatrices();
	quad->Draw();
	glUseProgram(0);
	glDepthMask(GL_TRUE);

}

void Renderer::DrawHeightmap() {
	SetCurrentShader(lightShader);
	SetShaderLight(*light);
	
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();
	heightMap->Draw();
	glUseProgram(0);

}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);
	float heightY = 256 * HEIGHTMAP_Y / 3.0f;
	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);
	modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ))
		* Matrix4::Scale(Vector3(heightX, 1, heightZ))
		* Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));
	UpdateShaderMatrices();
	quad->Draw();
	glUseProgram(0);

}