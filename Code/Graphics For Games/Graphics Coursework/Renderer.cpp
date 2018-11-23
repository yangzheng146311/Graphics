#include "Renderer.h"
#include"time.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	CubeRobot::CreateCube(); //Important!
	camera = new Camera();
	heightMap = new HeightMap("../../Textures/terrain.raw");
	_heightMap = new HeightMap("../../Textures/lalala.raw");
	hellData = new MD5FileData("../../Meshes/hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);
	hellData->AddAnim("../../Meshes/idle2.md5anim");
	hellData->AddAnim("../../Meshes/walk7.md5anim");
	hellData->AddAnim("../../Meshes/attack2.md5anim");
	hellNode->PlayAnim("../../Meshes/idle2.md5anim");
	
	quad = Mesh::GenerateQuad();
	_quad = Mesh::GenerateQuad();
	//camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 2500.0f, RAW_WIDTH * HEIGHTMAP_X));

	sphere = new OBJMesh();
	if (!sphere->LoadOBJMesh("../../Meshes/ico.obj")) {
		return;
	}
	rotation = 0.0f;

	camera->SetPosition(cam_S1_OriPos);
	camera->SetPitch(S1_CamOriPitch);
	camera->SetYaw(S1_CamOriYalw);

	pointLights = new Light[LIGHTNUM*LIGHTNUM];
	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light &l = pointLights[(x*LIGHTNUM) + z];
			float xPos = (RAW_WIDTH*HEIGHTMAP_X / (LIGHTNUM - 1)) * x;
			float zPos = (RAW_HEIGHT*HEIGHTMAP_Z / (LIGHTNUM - 1)) * z;
			l.SetPosition(Vector3(xPos, 100.0f, zPos));
			float r = 0.5f + (float)(rand() % 129) / 128.0f;
			float g = 0.5f + (float)(rand() % 129) / 128.0f;
			float b = 0.5f + (float)(rand() % 129) / 128.0f;
			l.SetColour(Vector4(r, g, b, 1.0f));
			float radius = (RAW_WIDTH*HEIGHTMAP_X / LIGHTNUM);
			l.SetRadius(radius);
		}
	}

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f-1000.0f), 500.0f, 
		(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)), 
		Vector4(1.0f, 1.0f, 1.0f, 1),
		(RAW_WIDTH * HEIGHTMAP_X) / 0.2f);
	/*CamOriX = camera->GetPosition().x;
	CamOriY = camera->GetPosition().y;
	CamOriZ = camera->GetPosition().z;*/
	curYaw = 0.0f;
	LightOriginRadius = light->GetRadius();
	LightOriginPosZ = light->GetPosition().z;
	LightOriginPosY = light->GetPosition().y;
	LightOriginPosX = light->GetPosition().x;

	root = new SceneNode();
	root->AddChild(new CubeRobot());
	hellShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	cubeShader = new Shader("../../Shaders/SceneVertex.glsl", "../../Shaders/SceneFragment.glsl");
	reflectShader = new Shader("../../Shaders/PerPixelVertex.glsl", "../../Shaders/reflectFragment.glsl");
	skyboxShader = new Shader("../../Shaders/skyboxVertex.glsl", "../../Shaders/skyboxFragment.glsl");
	lightShader = new Shader("../../Shaders/PerPixelVertex.glsl", "../../Shaders/PerPixelFragment.glsl");
	sceneShader = new Shader("../../Shaders/shadowscenevert.glsl", "../../Shaders/shadowscenefrag.glsl");
	_sceneShader = new Shader("../../Shaders/BumpVertex.glsl","../../Shaders/bufferFragment.glsl");
	combineShader = new Shader("../../Shaders/combinevert.glsl", "../../Shaders/combinefrag.glsl");
	shadowShader = new Shader("../../Shaders/shadowVert.glsl", "../../Shaders/shadowFrag.glsl");
	textShader= new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	pointlightShader = new Shader("../../Shaders/pointlightvertex.glsl","../../Shaders/pointlightfragment.glsl");
	particleShader = new Shader("../../Shaders/vertex.glsl",
		"../../Shaders/fragment.glsl",
		"../../Shaders/geometry.glsl");


	if (!cubeShader->LinkProgram()||!reflectShader->LinkProgram()|| !lightShader->LinkProgram() ||
		!skyboxShader->LinkProgram()||!sceneShader->LinkProgram()||!shadowShader->LinkProgram() || 
		!textShader->LinkProgram()||!particleShader->LinkProgram()
		||!_sceneShader->LinkProgram() || !combineShader->LinkProgram() || !pointlightShader->LinkProgram()) {
		return;

	}

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);
	emitter = new ParticleEmitter();

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



	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);
	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;
	//Generate our scene depth texture... 
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);
	//And now attach them to our FBOs 
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	


	floor = Mesh::GenerateQuad();
	floor->SetTexture(SOIL_load_OGL_texture("../../Textures/brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	floor->SetBumpMap(SOIL_load_OGL_texture("../../Textures/brickDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//floor->SetTexture(SOIL_load_OGL_texture("../../MyTextures/dryfloor.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//floor->SetBumpMap(SOIL_load_OGL_texture("../../MyTextures/dryfloorN.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	
	quad->SetTexture(SOIL_load_OGL_texture("../../MyTextures/water1.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetTexture(SOIL_load_OGL_texture("../../MyTextures/barren.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture("../../MyTextures/barren_normal.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	_heightMap->SetTexture(SOIL_load_OGL_texture(
		"../../Textures/Barren Reds.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	_heightMap->SetBumpMap(SOIL_load_OGL_texture(
		"../../Textures/Barren RedsDOT3.tga", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	cubeMap = SOIL_load_OGL_cubemap("../../MyTextures/purplenebula_lf.tga",
		"../../MyTextures/purplenebula_rt.tga",
		"../../MyTextures/purplenebula_up.tga",
		"../../MyTextures/purplenebula_dn.tga",
		"../../MyTextures/purplenebula_bk.tga",
		"../../MyTextures/purplenebula_ft.tga", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);


	if (!cubeMap || !quad->GetTexture() ||!heightMap->GetTexture() || !heightMap->GetBumpMap()) {
		return;
	}

	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
	SetTextureRepeating(_heightMap->GetTexture(), true);
	SetTextureRepeating(_heightMap->GetBumpMap(), true);

	waterRotate = 0.0f;
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
	
}

Renderer ::~Renderer(void) {
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	delete _heightMap;
	delete sphere;
	delete _quad;
	delete[] pointLights;
	delete camera;
	delete heightMap;
	delete quad;
	delete emitter;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete sceneShader;
	delete shadowShader;
	delete _sceneShader;
	delete combineShader;
	delete pointlightShader;
	delete floor;
	delete light;
	delete root;
	delete hellData;
	delete hellNode;
	delete basicFont;
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);
	
	currentShader = NULL;
	CubeRobot::DeleteCube(); //Also important!
	CubeRobot::DeleteSphere(); //Also important!


	//currentShader = 0;

}

void Renderer::UpdateScene(float msec) {
	
	
	
		/*if (stopCD<0.0f)
		{
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_K))
			{

				if (timeStop == false) {
					timeStop = true;
					stopCD = 160.0f;
				}
				if (timeStop == true)
				{
					timeStop = false;
					stopCD = 160.0f;
				}
			}
		}
	
	stopCD -= 1.0f;*/




	std::cout << timec << endl;
	curMsec = msec;
	if (timeStop == false)
	{
		timec += 1;
	}

	if (curScene == 1)
	{
		if (timec == 100) hellNode->PlayAnim("../../Meshes/walk7.md5anim");
		if (timec > 100 && timeStop == false)
		{
			if (hellNightX > -800)
			{
				hellNightX -= 15;
			}

			else
				if (aniWalk == true)
				{
					aniWalk = false;
					hellNode->PlayAnim("../../Meshes/idle2.md5anim");

				}
		}

		if (timec > 200 && timeStop == false)
		{
			if (aniAttack == false)
			{
				hellNode->PlayAnim("../../Meshes/attack2.md5anim");
				aniAttack = true;

			}
		}

		if (timec > 400 && timeStop == false)
		{

			if (aniIdle == false)
			{

				hellNode->PlayAnim("../../Meshes/idle2.md5anim");
				aniIdle = true;

			}
			hellNightY += 8;
		}

		if (timec > 450 && timeStop == false)
		{
			curScene++;
			hellNode->PlayAnim("../../Meshes/idle2.md5anim");
			if (curScene == 2)
			{
				hellNightY = 2000.0f;
				hellNightX = 0;
				hellNightZ = 0;
				camera->SetPosition(cam_S2_OriPos);
				camera->SetPitch(S2_CamOriPitch);
				camera->SetYaw(S2_CamOriYalw);
			}
			timec = 0;


		}
		
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT))
		{
			curScene = 2;
			hellNode->PlayAnim("../../Meshes/idle2.md5anim");
			
				hellNightY = 2000.0f;
				hellNightX = 0;
				hellNightZ = 0;
				camera->SetPosition(cam_S2_OriPos);
				camera->SetPitch(S2_CamOriPitch);
				camera->SetYaw(S2_CamOriYalw);
			    timec = 0;
		}
	}

	if (curScene == 2) {

		if(hellNightY>100 && timeStop == false)
		hellNightY -= 8;
		
		if (timec == 300)
			lightOn = true;

		/*if (timec > 600)
		{
			curScene++;
			timec = 0;
		}*/

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT))
		{
			curScene=1;
			hellNode->PlayAnim("../../Meshes/idle2.md5anim");
			light->SetPosition(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f - 1000.0f), 500.0f,
					(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)));
			light->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1));
			light->SetRadius((RAW_WIDTH * HEIGHTMAP_X) / 0.2f);
			lightOn = false;
			hellNightY = 110.0f;
			hellNightX = 0;
			hellNightZ = 0;
			camera->SetPosition(cam_S1_OriPos);
			camera->SetPitch(S1_CamOriPitch);
			camera->SetYaw(S1_CamOriYalw);
			timec = 0;
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT))
		{
			    curScene=3;
				camera->SetPosition(cam_S3_OriPos);
				camera->SetPitch(S3_CamOriPitch);
				camera->SetYaw(S3_CamOriYalw);
			    timec = 0;
		}
		
	}

	if (curScene == 3) {

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT))
		{
			curScene = 2;
			hellNode->PlayAnim("../../Meshes/idle2.md5anim");
			hellNightY = 2000.0f;
			hellNightX = 0;
			hellNightZ = 0;
			camera->SetPosition(cam_S2_OriPos);
			camera->SetPitch(S2_CamOriPitch);
			camera->SetYaw(S2_CamOriYalw);
			timec = 0;
		}
	
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE))
	{
		if (camMove == true)
			camMove = false;
		else
			camMove = true;
	}

	//CamMove
	{
		/*if (camMove)
		{
			curYaw += 1.0f*dir;
			if (curYaw > 720.0f || curYaw < 0.0f)
			{

				dir *= -1;
			}
		}*/
		//float CamCurX = CamOriZ * sin(curYaw*PI / 180.0f) + CamOriX;
		//float CamCurZ = -CamOriZ * cos(curYaw*PI / 180.0f) + CamOriZ;

		/*camera->SetPosition(Vector3(CamOriX, CamOriY, CamOriZ));
		float nowYaw = CamOriYaw + curYaw;
		camera->SetYaw(nowYaw*0.1f);
		camera->SetPitch(nowYaw*-0.1f);*/

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

	}

	if (lightOn == true)
	{

		//Light Radius Change
		{

			float r = light->GetRadius();
			if (r > LightOriginRadius)
			{

				lightOff = true;
			}

			if (r < 0.0f)
			{

				lightOff = false;
				curScene++;
				timec = 0;
				lightOn = false;
				camera->SetPosition(cam_S3_OriPos);
				camera->SetPitch(S3_CamOriPitch);
				camera->SetYaw(S3_CamOriYalw);
			}


			if (lightOff == true)
			{
				r -= 50;
			}
			else
			{
				r += 50;
			}

			light->SetRadius(r);
		}

		//Light Color Change
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

			if (pos.x > LightOriginPosX)
			{

				lightRight = false;
			}

			if (pos.x < LightOriginPosX - 4000.0f)
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
			if (pos.z > LightOriginPosZ)
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
			light->SetPosition(pos);
		}
	}

		
	emitter->Update(msec);
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	root->Update(msec);
	hellNode->Update(msec);
	rotation = msec * 0.01f;
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

	if(curScene==1)  DrawScene_A ();

	if(curScene==2)   DrawScene_B();

	if (curScene == 3)  DrawScene_C();

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
	if (curScene == 1)
	{
		DrawFloor();
	}
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
	if (curScene == 1)
	{
		DrawFloor();
	}
	DrawMesh();
	
	glUseProgram(0);
}

void Renderer::DrawFPS()
{
	SetCurrentShader(textShader);
	//And turn on texture unit 0
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	//Render function to encapsulate our font rendering!
	string status = "";
	if (timeStop == false) status = "Running";
	else status = "Pause";
		DrawText("FPS:" + FloatToString(1000.0f / curMsec)+" "+status+" "+ FloatToString(timec) + " " + FloatToString(stopCD), Vector3(0, 0, 0), 16.0f);
	DrawText("hahaha", Vector3(-54,11500,49), 64.0f, true);

	glUseProgram(0);	//That's everything!
}

void Renderer::DrawParticle()
{
	glClearColor(0, 0, 0, 1);
	
	SetCurrentShader(particleShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	SetShaderParticleSize(emitter->GetParticleSize());
	
	emitter->SetParticleSize(8.0f);
	emitter->SetParticleVariance(1.0f);
	emitter->SetLaunchParticles(128.0f);
	emitter->SetParticleLifetime(2000.0f);
	emitter->SetParticleSpeed(1.0f);
	

	/*modelMatrix.ToIdentity();
	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);*/
	UpdateShaderMatrices();


	emitter->Draw();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(0);
}

void Renderer::SetShaderParticleSize(float f) {
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), f);
}

string Renderer::FloatToString(float msec)
{
	char buf[10];
	
	gcvt(msec, 6, buf);
	string str(buf);
	return str;
}

void Renderer::DrawMesh() {
	modelMatrix.ToIdentity();
	modelMatrix.SetScalingVector(Vector3(4, 4, 4));
	
	modelMatrix.SetPositionVector(Vector3(hellNightX, hellNightY, hellNightZ));
	
	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

   
	hellNode->Draw(*this);
}

void Renderer::DrawFloor() {
	modelMatrix.ToIdentity();
	modelMatrix = Matrix4::Rotation(90, Vector3(1, 0, 0))*Matrix4::Scale(Vector3(3600, 3600, 1));
	modelMatrix.SetPositionVector(Vector3(0, 110, 0));
	
	
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

void Renderer::DrawScene_A()
{
	/*camera->SetPosition(Vector3(-429, 499, 1947));
	camera->SetPitch(4);
	camera->SetYaw(352);*/

	DrawSkybox();

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();
	DrawFPS();

	DrawShadowScene(); // First render pass ...
	DrawCombinedScene(); // Second render pass ...
	/*if(timec>450)
	DrawParticle();*/
}

void Renderer::DrawScene_B()
{

	DrawSkybox();
	DrawHeightmap();
	DrawFPS();
	DrawWater();
	
	DrawShadowScene(); // First render pass ...
	DrawCombinedScene(); // Second render pass ...

	if (timec > 300)
	{
		DrawCube();
		DrawParticle();
	}
	
}

void Renderer::DrawScene_C()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawFPS();
	
	FillBuffers();
	DrawPointLights();
	CombineBuffers();
	
}

void Renderer::GenerateScreenTexture(GLuint &into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0,
		depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8,
		width, height, 0,
		depth ? GL_DEPTH_COMPONENT : GL_RGBA,
		GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(_sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();
	_heightMap->Draw();
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPointLights() {

	SetCurrentShader(pointlightShader);
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"depthTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"normTex"), 4);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(),
		"pixelSize"), 1.0f / width, 1.0f / height);
	Vector3 translate = Vector3((RAW_HEIGHT*HEIGHTMAP_X / 2.0f), 500,
		(RAW_HEIGHT*HEIGHTMAP_Z / 2.0f));
	Matrix4 pushMatrix = Matrix4::Translation(translate);
	Matrix4 popMatrix = Matrix4::Translation(-translate);
	for (int x = 0; x < LIGHTNUM; ++x) {

		for (int z = 0; z < LIGHTNUM; ++z) {

			Light &l = pointLights[(x*LIGHTNUM) + z];
			float radius = l.GetRadius();
			modelMatrix =
				pushMatrix *
				Matrix4::Rotation(rotation, Vector3(0, 1, 0)) *
				popMatrix *
				Matrix4::Translation(l.GetPosition()) *
				Matrix4::Scale(Vector3(radius, radius, radius));
			l.SetPosition(modelMatrix.GetPositionVector());
			SetShaderLight(l);
			UpdateShaderMatrices();
			float dist = (l.GetPosition() - camera->GetPosition()).Length();
			if (dist < radius) {//camera is inside the light volume! 
				glCullFace(GL_FRONT);
			}
			else {
				glCullFace(GL_BACK);
			}
			sphere->Draw();
		}
	}
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

}

void Renderer::CombineBuffers() {
	SetCurrentShader(combineShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"emissiveTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"specularTex"), 4);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);
	_quad->Draw();
	glUseProgram(0);
}