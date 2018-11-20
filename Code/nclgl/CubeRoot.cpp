#include "CubeRoot.h"

 Mesh* CubeRobot::cube = NULL;

 CubeRobot::CubeRobot(void) {
	 //Optional , uncomment if you want a local origin marker!
		 //SetMesh(cube);
	

	 //expand 100times scale

SceneNode*body = new SceneNode(cube, Vector4(1, 0, 0, 1)); //Red!
//body->SetModelScale(Vector3(10, 15, 50));
body->SetModelScale(Vector3(60, 150, 80));
body->SetTransform(Matrix4::Translation(Vector3(0, 1500,0 )));
AddChild(body);

head = new SceneNode(cube, Vector4(0, 1, 0, 1)); //Green!
head->SetModelScale(Vector3(50, 50, 50));
head->SetTransform(Matrix4::Translation(Vector3(0, 300, 0)));
body->AddChild(head);

leftArm = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue!
leftArm->SetModelScale(Vector3(50, -100, 30));
leftArm->SetTransform(Matrix4::Translation(Vector3(-120, 180, -1)));
body->AddChild(leftArm);

rightArm = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue!
rightArm->SetModelScale(Vector3(50, -100, 30));
rightArm->SetTransform(Matrix4::Translation(Vector3(120, 180, -1)));
body->AddChild(rightArm);

SceneNode* leftLeg = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue!
leftLeg->SetModelScale(Vector3(30, -175, 30));
leftLeg->SetTransform(Matrix4::Translation(Vector3(-80, 0, 0)));
body->AddChild(leftLeg);

SceneNode* rightLeg = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue!
rightLeg->SetModelScale(Vector3(30, -175, 30));
rightLeg->SetTransform(Matrix4::Translation(Vector3(80, 0, 0)));
body->AddChild(rightLeg);

body->SetBoundingRadius(15.0f); 
head->SetBoundingRadius(5.0f); 
leftArm->SetBoundingRadius(18.0f); 
rightArm->SetBoundingRadius(18.0f); 
leftLeg->SetBoundingRadius(18.0f); 
rightLeg->SetBoundingRadius(18.0f);
}
 void CubeRobot::Update(float msec) {
	  transform = transform *
Matrix4::Rotation(msec / 10.0f, Vector3(0, 1, 0));

head->SetTransform(head->GetTransform() *
Matrix4::Rotation(-msec / 10.0f, Vector3(0, 1, 0)));


leftArm->SetTransform(leftArm->GetTransform() *
Matrix4::Rotation(-msec / 10.0f, Vector3(1, 0, 0)));

rightArm->SetTransform(rightArm->GetTransform() *
Matrix4::Rotation(msec / 10.0f, Vector3(1, 0, 0)));

SceneNode::Update(msec);
	 
 }
