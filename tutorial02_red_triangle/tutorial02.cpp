// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "common/shader.hpp"

void drawTriangleFunction(GLuint programID, GLuint vertexbuffer, GLuint MatrixIDtranslate, GLuint MatrixIDScale, GLuint MatrixIDRotate, glm::mat4 translateMatrix, glm::mat4 scaleMatrix, glm::mat4 rotateMatrix) {
	glUseProgram(programID);


	// use translate
	glUniformMatrix4fv(MatrixIDtranslate, 1, GL_FALSE, &translateMatrix[0][0]);

	// use scale
	glUniformMatrix4fv(MatrixIDScale, 1, GL_FALSE, &scaleMatrix[0][0]);

	//use rotation
	glUniformMatrix4fv(MatrixIDRotate, 1, GL_FALSE, &rotateMatrix[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
	glDisableVertexAttribArray(0);
};

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
	GLuint programID1 = LoadShaders( "SimpleVertexShader1.vertexshader", "SimpleFragmentShader.fragmentshader" );
	//create translate matrix T(0.5, 0.0, 0.0)
	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.5f, 0.0f, 0.0f));
	GLuint MatrixIDtranslate = glGetUniformLocation(programID, "translationMatrix");
	
	//create scaling matrix S(0.5, 0.5, 0.5)
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.5f, 0.5f, 0.5f));
	GLuint MatrixIDScale = glGetUniformLocation(programID, "scaleMatrix");

	//create rotation matrix to rotate 90 degrees around Z
	glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
	GLuint MatrixIDRotate = glGetUniformLocation(programID, "rotationMatrix");

	glm::mat4 translateMatrix1 = glm::translate(glm::mat4(1.f), glm::vec3(-2.f, 0.0f, 0.0f));
	GLuint MatrixIDtranslate1 = glGetUniformLocation(programID1, "translationMatrix");
	
	//create scaling matrix S(0.5, 0.5, 0.5)
	glm::mat4 scaleMatrix1 = glm::scale(glm::mat4(1.f), glm::vec3(0.5f, 0.5f, 0.5f));
	GLuint MatrixIDScale1 = glGetUniformLocation(programID1, "scaleMatrix");

	// glm::mat4 rotateMatrix1 = glm::rotate(glm::mat4(1.f), (float)glfwGetTime(), glm::vec3(0.f, 0.f, 1.f));
	// GLuint MatrixIDRotate1 = glGetUniformLocation(programID1, "rotationMatrix");


	static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	do{

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );
		glm::mat4 rotateMatrix1 = glm::rotate(glm::mat4(1.f), (float)glfwGetTime(), glm::vec3(0.f, 0.f, 1.f));
		GLuint MatrixIDRotate1 = glGetUniformLocation(programID1, "rotationMatrix");
		drawTriangleFunction(programID,vertexbuffer,MatrixIDtranslate,MatrixIDScale,MatrixIDRotate, translateMatrix,scaleMatrix,rotateMatrix);
		drawTriangleFunction(programID1,vertexbuffer,MatrixIDtranslate1,MatrixIDScale1,MatrixIDRotate1, translateMatrix1,scaleMatrix1,rotateMatrix1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);
	glDeleteProgram(programID1);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

