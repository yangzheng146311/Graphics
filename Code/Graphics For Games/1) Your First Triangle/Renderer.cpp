#include"./../2) Matrix Transformations/Renderer.h"

 Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	 triangle = Mesh::GenerateTriangle();
	
		 currentShader = new Shader(SHADERDIR"basicVertex.glsl",SHADERDIR"colourFragment.glsl");
	    

	
		 if (!currentShader ->LinkProgram()) {
		return;
		
	      }
	
		 init = true;

		 SwitchToOrthographic();
	
}
 Renderer ::~Renderer(void) {
	 delete triangle;
 }
 void Renderer::RenderScene() {
	  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	  glClear(GL_COLOR_BUFFER_BIT);
	 
     glUseProgram(currentShader -> GetProgram());
	  triangle -> Draw();
	  glUseProgram(0);
	 
      SwapBuffers();

	 





	 
 }

 void Renderer::SwitchToPerspective() { 
	 projMatrix = Matrix4::Perspective(1.0f, 10000.0f,  (float)width / (float)height, 45.0f); 
 }


 void Renderer::SwitchToOrthographic() { 
	 projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f,  width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f); 
 }
