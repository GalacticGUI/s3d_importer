#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp> //for matrix transformation functions
#include "glslprogram.h"
#include "TextureManager.h"
#include "Mesh.h"


static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if(!glfwInit())
		exit(EXIT_FAILURE);

	window = glfwCreateWindow(1000, 800, "Blender Object", NULL, NULL);
	if(!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);// now we have an OpenGL context for this thread.

	//use GLEW to initialiaze modern opengl functionality supported by the GPU drivers
	glewInit();

	//Make Texture Manager
	TextureManager* texManager = new TextureManager();

	//load shaders
	GLSLProgram shaders;
	shaders.compileShaderFromFile("vertex.glsl", GLSLShader::GLSLShaderType::VERTEX);
	shaders.compileShaderFromFile("fragment.glsl", GLSLShader::GLSLShaderType::FRAGMENT);

	//bind attributes for the shader layout BEFORE linking the shaders!
	//We only need to do this if we can't use the layout command from within the shader code.
	shaders.bindAttribLocation(0, "position");
	shaders.bindAttribLocation(1, "normal");
	shaders.bindAttribLocation(2, "texUV");

	shaders.link();
	shaders.use();

	//~~~~~~~~~~~~MAKE MESH HERE~~~~~~~~~~~~~~//
	Mesh* mesh = new Mesh(texManager, &shaders);

	glfwSetKeyCallback(window, key_callback);

	// Enable blending
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);								// enables proper 3D depth drawing
	glEnable(GL_CULL_FACE);									// backs of triangles are no longer drawn
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Primitive restart setup for triangle strips
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFF); //0xFFFF, maximun value
	
	//view matrix to transform the camera
	glm::mat4 viewMatrix;
	//projection matrix to project the scene onto the monitor
	glm::mat4 projectionMatrix;

	//...and an accumulator for rotatation:
	float angle = 0.f;

	//timer vars
	double currentTime = glfwGetTime();
	double previousTime = currentTime;
	double timePassed;

	//location of the mesh object
	glm::vec3 cameraFocusPosition(0.f, 0.5f, -5.f);
	mesh->position = cameraFocusPosition;

	while(!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);	//uncomment to test
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//update projection (needed if window size can be modified)
		projectionMatrix = glm::perspective(45.0f, ratio, 0.1f, 10000.f);

		//send the matrix to the shader
		shaders.setUniform("projectionMatrix", projectionMatrix);

		//update camera position via the view matrix
		viewMatrix = glm::lookAt(
			glm::vec3(-1.f, 1.f, 1.f), //where is the EYE of the camera
			cameraFocusPosition, //a point we are looking at -> the mesh location
			glm::vec3(0.f, 1.f, 0.f) // up vector of the camera
			);

		//send the matrix to the shader
		shaders.setUniform("viewMatrix", viewMatrix);

		//light position
		shaders.setUniform("lightPos", glm::vec3(-1.0f, -1.0f, 10.0f));

		//update our variable for time before passing into cube update
		currentTime = glfwGetTime();
		timePassed = currentTime - previousTime;
		if (timePassed > 0) previousTime = currentTime;

		mesh->Update(timePassed);	
		mesh->Draw();
		
		glfwSwapBuffers(window); //display the graphics buffer to the screen
		glfwPollEvents(); //prime the message pump that GLFW uses for input events
	}

	delete mesh;
	delete texManager;

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}