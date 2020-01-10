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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "common/shader.hpp"
#include "common/controls.hpp"

int main( void )
{
	//Begin OpenGL Context Creation process 
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
	window = glfwCreateWindow( 800, 600, "Tutorial 01", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//End of OpenGL Context creation process

	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	

	static const GLfloat g_vertex_buffer_data[] = {
		//Position			//Color
	   -1.0f, -1.0f, 1.0f,  1.0f, 0.0f, 0.0f, // Bottom right #0 #Red
	   1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom left	 #1 #Blue
	   1.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // Top right  #2 #Green
	   -1.0f, 1.0f, 1.0f,  0.7f, 0.0f, 0.5f, // Top Left  #3 #Purple

	   -1.0f, -1.0f,-1.0f, 0.5f,  0.7f,  0.0f, // #4
	   1.0f, -1.0f, -1.0f, 0.6f,  0.1f,  0.4f, // #5
	   1.0f, 1.0f, -1.0f, 0.3f,  0.4f,  0.8f, //#6
	   -1.0f, 1.0f, -1.0f, 0.8f,  0.5f,  0.2f //#7
	};

	unsigned int indices[] = {  // note that we start from 0!
        // front
		0, 2, 3, // first Triangle
		1, 2, 0, // second triangle
		//right
		1, 5, 6,
		1, 2, 6,
		//back
		7, 6, 5,
		5, 4, 7,
		//left
		4, 0, 3,
		3, 7, 4,
		//bottom
		4, 5, 1,
		1, 0, 4,
		//top
		3, 2, 6,
		3, 7, 6
    };
 

	// Draw triangle 
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// This will identify our vertex buffer
	//GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	//glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	

	// Draw Triangle with draw Element
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	

	//Transformations
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	// glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// // Or, for an ortho camera :
	// //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
	// // Camera matrix
	// glm::mat4 View = glm::lookAt(
	// 	glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
	// 	glm::vec3(0, 0, 0), // and looks at the origin
	// 	glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	// );
	// // Model matrix : an identity matrix (model will be at the origin)
	// glm::mat4 Model = glm::mat4(1.0f);
	// // Our ModelViewProjection : multiplication of our 3 matrices
	// glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around



	do{
		// Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
		glClear( GL_COLOR_BUFFER_BIT );
		
		// Use our shader
		glUseProgram(programID);		
		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.f), (float)glfwGetTime(), glm::vec3(0.f, 0.f, 1.f));
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix * rotateMatrix;
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		
		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		// To take Posiion
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			6 * sizeof(float),                  // stride
			(void*)0            // array buffer offset
		);
		// To take Color
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			6 * sizeof(float),                  // stride
			(void*)(3 * sizeof(float))            // array buffer offset
		);
		glEnableVertexAttribArray(1);

		// Draw traiangle Draw Array
		//glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
		//glDisableVertexAttribArray(0);
		
		// Draw triangle with Draw Element 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_INT, 0);


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
