#pragma once
#include "../../nclgl/SceneNode.h" 
#include"../../nclgl/OBJMesh.h"




 class CubeRobot : public SceneNode {
 public:
	 CubeRobot(void);
	 ~CubeRobot(void) {};
	 virtual void Update(float msec);
	
	 static void CreateCube() {
		  OBJMesh *m = new OBJMesh();
		  m->LoadOBJMesh("../../Meshes/sphere.obj");

		  OBJMesh *m2 = new OBJMesh();
		  m2->LoadOBJMesh("../../Meshes/cube.obj");


		  sphere = m;
		  cube = m2;
		 
	 }
	 static void DeleteCube() { delete cube; }


	 static void DeleteSphere() {
		 delete sphere;
	 }
	
 protected:
	 static Mesh* cube;
	 static Mesh* sphere;
	 SceneNode* head;
	 SceneNode* leftArm;
	 SceneNode* rightArm;
	 SceneNode* leftLeg;
	 SceneNode* rightLeg;


	 
	
};