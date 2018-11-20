#include "CubeRoot.h"

 Mesh* CubeRobot::cube = NULL;
 Mesh* CubeRobot::sphere = NULL;
 

 CubeRobot::CubeRobot(void) {
	 //Optional , uncomment if you want a local origin marker!
		 //SetMesh(cube);
	

	 //expand 100times scale

SceneNode*body = new SceneNode(sphere, Vector4(1, 1, 1, 1)); //Red!
//body->SetModelScale(Vector3(10, 15, 50));
body->SetModelScale(Vector3(15, 15, 15));
body->SetTransform(Matrix4::Translation(Vector3(0, 500,0 )));
AddChild(body);

head = new SceneNode(cube, Vector4(0.5f, 0.6f, 0.4f, 1)); //Green!
head->SetModelScale(Vector3(5, 5, 5));
head->SetTransform(Matrix4::Translation(Vector3(0, 30, 0)));
body->AddChild(head);

leftArm = new SceneNode(cube, Vector4(1, 1, 0, 1)); //Yellow
leftArm->SetModelScale(Vector3(-10, -10, -10));
leftArm->SetTransform(Matrix4::Translation(Vector3(-30, -30, -30)));
body->AddChild(leftArm);

rightArm = new SceneNode(cube, Vector4(1, 0, 0.1f, 1)); //red
rightArm->SetModelScale(Vector3(10, -10, 10));
rightArm->SetTransform(Matrix4::Translation(Vector3(30, -30, -30)));
body->AddChild(rightArm);

leftLeg = new SceneNode(cube, Vector4(0, 1, 0, 1)); //purple
leftLeg->SetModelScale(Vector3(10, -10, 10));
leftLeg->SetTransform(Matrix4::Translation(Vector3(-30, -30, 30)));
body->AddChild(leftLeg);

rightLeg = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue!
rightLeg->SetModelScale(Vector3(10, -10, 10));
rightLeg->SetTransform(Matrix4::Translation(Vector3(30, -30, 30)));
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




leftLeg->SetTransform(leftLeg->GetTransform() *
	Matrix4::Rotation(-msec / 10.0f, Vector3(0, 0, 1)));

rightLeg->SetTransform(rightLeg->GetTransform() *
	Matrix4::Rotation(msec / 10.0f, Vector3(0, 0, 1)));

leftArm->SetTransform(leftArm->GetTransform() *
Matrix4::Rotation(-msec / 10.0f, Vector3(1, 0, 0)));

rightArm->SetTransform(rightArm->GetTransform() *
Matrix4::Rotation(msec / 10.0f, Vector3(1, 0, 0)));



SceneNode::Update(msec);
	 
 }
