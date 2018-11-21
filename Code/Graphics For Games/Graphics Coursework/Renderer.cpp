#include "Renderer.h"
#include"time.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	CubeRobot::CreateCube(); //Important!
	camera = new Camera();
	heightMap = new HeightMap("../../Textures/terrain.raw");

	hellData = new MD5FileData("../../Meshes/hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);
	hellData->AddAnim("../../Meshes/idle2.md5anim");
	hellNode->PlayAnim("../../Meshes/idle2.md5anim");

	quad = Mesh::GenerateQuad();
	//camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 2500.0f, RAW_WIDTH * HEIGHTMAP_X));


	camera->SetPosition(Vector3(CamOriX, CamOriY,CamOriZ));
	camera->SetPitch(CamOriPitch);
	camera->SetYaw(CamOriYaw);
	

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f-1000.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f -1000.f)), Vector4(1.0f, 1.0f, 1.0f, 1), (RAW_WIDTH * HEIGHTMAP_X) / 0.2f);
	CamOriX = camera->GetPosition().x;
	CamOriY = camera->GetPosition().y;
	CamOriZ = camera->GetPosition().z;
	curYaw = 0.0f;
	LightOriginRadius = light->GetRadius();
	LightOriginPosZ = light->GetPosition().z;
	LightOriginPosY = light->GetPosition().y;
	LightOriginPosX = light->GetPosition().x;

	root = new SceneNode();
	root->AddChild(new CubeRobot());
	

	cubeShader = new Shader("../../Shaders/SceneVertex.glsl", "../../Shaders/SceneFragment.glsl");
	reflectShader = new Shader("../../Shaders/PerPixelVertex.glsl", "../../Shaders/reflectFragment.glsl");
	skyboxShader = new Shader("../../Shaders/skyboxVertex.glsl", "../../Shaders/skyboxFragment.glsl");
	lightShader = new Shader("../../Shaders/PerPixelVertex.glsl", "../../Shaders/PerPixelFragment.glsl");
	sceneShader = new Shader("../../Shaders/shadowscenevert.glsl", "../../Shaders/shadowscenefrag.glsl");
	shadowShader = new Shader("../../Shaders/shadowVert.glsl", "../../Shaders/shadowFrag.glsl");
	textShader= new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");


	if (!cubeShader->LinkProgram()||!reflectShader->LinkProgram()|| !lightShader->LinkProgram() ||
		!skyboxShader->LinkProgram()||!sceneShader->LinkProgram()||!shadowShader->LinkProgram() || !textShader->LinkProgram()) {
		return;

	}

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);


	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	floor = Mesh::GenerateQuad();
	floor->SetTexture(SOIL_load_OGL_texture("../../Textures/brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	floor->SetBumpMap(SOIL_load_OGL_texture("../../Textures/brickDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	floor->SetBumpMap(SOIL_load_OGL_texture("../../Textures/brickDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	quad->SetTexture(SOIL_load_OGL_texture("../../MyTextures/water1.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetTexture(SOIL_load_OGL_texture("../../MyTextures/barren.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture("../../MyTextures/barren_normal.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
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
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

Renderer ::~Renderer(void) {
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

	delete camera;
	delete heightMap;
	delete quad;
	
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete sceneShader;
	delete shadowShader;
	delete floor;
	delete light;
	delete root;
	delete hellData;
	delete hellNode;
	delete basicFont;

	
	currentShader = NULL;
	CubeRobot::DeleteCube(); //Also important!
	CubeRobot::DeleteSphere(); //Also important!


	currentShader = 0;

}

void Renderer::UpdateScene(float msec) {
	
	curMsec = msec;
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
		
		if (pos.x > LightOriginPosX  )
		{

			lightRight = false;
		}

		if (pos.x < LightOriginPosX-4000.0f)
		{

			lightRight = true;
		}

		if (pos.y > LightOriginPosY)
		{

			lightUp = false;
		}

		if (pos.y < LightOriginPosY - 4000.0f)
		{

			lightUp = true;;
		}
		if (pos.z > LightOriginPosZ )
		{

			lightFront = false;
		}
		if (pos.z < LightOriginPosZ - 4000.0f)
		{

			lightFront = true;
		}
		if (lightRight == true) pos.x += 10.f;
		else pos.x -= 10.0f;
		/*if (lightUp == true) pos.y += 10.f;
		else pos.y -= 10.0f;*/
		if (lightFront == true) pos.z += 10.f;
		else pos.z -= 10.0f;

		/*curYaw += 0.1f;
		if (curYaw > 360.0f) curYaw=0.0f;
		float CamCurX = CamOriZ * sin(curYaw*PI / 180.0f) + CamOriX;
		float CamCurZ = -CamOriZ * cos(curYaw*PI / 180.0f) + CamOriZ;
		
		camera->SetPosition(Vector3(CamCurX, CamOriY, CamCurZ));
		float nowYaw = CamOriYaw + curYaw;
		camera->SetYaw(nowYaw);*/
		
		
	
		//light->SetPosition(pos);
		
		
	}
	
	/*curAngel += 1.0f;
	if (curAngel > 360.0f) curAngel = 0.0f;

	curPitch += 0.001f;
	if (curPitch > 90.0f) curPitch = 0.0f;
	double camX = CamOriX * cos(curAngel*PI/180.0f);
	double camZ= CamOriX * sin(curAngel*PI / 180.0f);
	double camPitch = CamOriPitch += curPitch;*/
	
	
	/*camera->SetPosition(Vector3((float)camX, 1000.0f, (float)camZ));
	camera->SetYaw((float)camPitch);
	*/
	camera->UpdateCamera(msec);

	viewMatrix = camera->BuildViewMatrix();
	root->Update(msec);
	hellNode->Update(msec);

	waterRotate += msec / 1000.0f;
	cout <<"pitch:"<< camera->GetPitch() << endl;
	cout << "yalw:"<<camera->GetYaw() << endl;
	cout <<"x:"<< camera->GetPosition().x << endl;
	cout << "y:" << camera->GetPosition().y << endl;
	cout << "z:" << camera->GetPosition().z << endl;
	//cout<<msec<<endl;
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkybox();
	DrawHeightmap();
	DrawFPS();

	DrawWater();
	DrawCube();
	DrawShadowScene(); // First render pass ...
	DrawCombinedScene(); // Second render pass ...
	
	
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
	modelMatrix.SetPositionVector(Vector3(-2000, 0, -2000));
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
	modelMatrix.SetPositionVector(Vector3(0, 100, -100));
	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));
	UpdateShaderMatrices();
	quad->Draw();
	glUseProgram(0);

}

void Renderer::DrawCube()
{
	SetCurrentShader(cubeShader);
	SetShaderLight(*light);
	glUseProgram(currentShader->GetProgram());
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 1);
	DrawNode(root);
	glUseProgram(0);
}

void Renderer::DrawNode(SceneNode*n) {
	
	if (n->GetMesh()) {
		Matrix4 transform = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(
			glGetUniformLocation(currentShader->GetProgram(),
				"modelMatrix"), 1, false, (float*)&transform);
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(),
			"nodeColour"), 1, (float*)&n->GetColour());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"useTexture"), (int)n->GetMesh()->GetTexture());
		n->Draw(*this);
	}
	for (vector <SceneNode*>::const_iterator
		i = n->GetChildIteratorStart();
		i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);
	viewMatrix = Matrix4::BuildViewMatrix(
	light->GetPosition(), Vector3(0, 0, 0));
	textureMatrix = biasMatrix * (projMatrix * viewMatrix);
	UpdateShaderMatrices();
	DrawFloor();
	DrawMesh();
	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene() {
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	SetShaderLight(*light);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();
	DrawFloor();
	DrawMesh();
	
	glUseProgram(0);
}

void Renderer::DrawFPS()
{
	SetCurrentShader(textShader);
	//And turn on texture unit 0
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	//Render function to encapsulate our font rendering!
	
	DrawText("FPS:"+FrameRateToString(1000.0f/curMsec), Vector3(0, 0, 0), 16.0f);
	DrawText("hahaha", Vector3(-54,11500,49), 64.0f, true);

	glUseProgram(0);	//That's everything!
}

string Renderer::FrameRateToString(float msec)
{
	char buf[10];
	
	gcvt(msec, 6, buf);
	string str(buf);
	return str;
}

void Renderer::DrawMesh() {
	modelMatrix.ToIdentity();
	modelMatrix.SetPositionVector(Vector3(500, 220, 800));
	modelMatrix.SetScalingVector(Vector3(2, 2, 2));
	
	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);
	hellNode->Draw(*this);
}

void Renderer::DrawFloor() {
	modelMatrix.ToIdentity();
	modelMatrix = Matrix4::Rotation(90, Vector3(1, 0, 0))*Matrix4::Scale(Vector3(650, 650, 1));
	modelMatrix.SetPositionVector(Vector3(-100, 220, 500));
	//modelMatrix.SetScalingVector(Vector3(1000, 1000, 1000));
	
	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);
	floor->Draw(); 

}

void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
		//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text,*basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if(perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size,size,1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)width / (float)height, 45.0f);
	}
	else{	
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x,height-position.y, position.z)) * Matrix4::Scale(Vector3(size,size,1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f,1.0f,(float)width, 0.0f,(float)height, 0.0f);
	}
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!
}


