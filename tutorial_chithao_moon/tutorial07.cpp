// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

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
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
void createquad(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs)
{
	glm::vec3 vertex1 = glm::vec3(-1.0f, -1.0f, 0.0f);
	vertices.push_back(vertex1);
	glm::vec3 vertex2 = glm::vec3(1.0f, -1.0f, 0.0f);
	vertices.push_back(vertex2);
	glm::vec3 vertex3 = glm::vec3(1.0f, 1.0f, 0.0f);
	vertices.push_back(vertex3);
	glm::vec3 vertex4 = glm::vec3(-1.0f, -1.0f, 0.0f);
	vertices.push_back(vertex4);
	glm::vec3 vertex5 = glm::vec3(1.0f, 1.0f, 0.0f);
	vertices.push_back(vertex5);
	glm::vec3 vertex6 = glm::vec3(-1.0f, 1.0f, 0.0f);
	vertices.push_back(vertex6);
	glm::vec2 uv1 = glm::vec2(0.0f,0.0f);
	uvs.push_back(uv1);
	glm::vec2 uv2 = glm::vec2(1.0f,0.0f);
	uvs.push_back(uv2);
	glm::vec2 uv3 = glm::vec2(1.0f,1.0f);
	uvs.push_back(uv3);
	glm::vec2 uv4 = glm::vec2(0.0f,0.0f);
	uvs.push_back(uv4);
	glm::vec2 uv5 = glm::vec2(1.0f,1.0f);
	uvs.push_back(uv5);
	glm::vec2 uv6 = glm::vec2(0.0f,1.0f);
	uvs.push_back(uv6);
	
}
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
	window = glfwCreateWindow( 1024, 768, "Tutorial 07 - Model Loading", NULL, NULL);
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
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
	GLuint programID1 = LoadShaders( "Diffuse.vertexshader", "Diffuse.fragmentshader");
	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint lightPos_location = glGetUniformLocation(programID1, "lightPos_worldspace");
	GLuint viewPos_location = glGetUniformLocation(programID1, "viewPos_worldspace");
	GLuint modelMatrix = glGetUniformLocation(programID1, "ModelMatrix");
	GLuint MatrixID1 = glGetUniformLocation(programID1, "MVP");
	// Load the texture
	// GLuint Texture = loadPNG("Diffuse_2K.png");
	
	glm::vec3 lightPos = glm::vec3(3.f,3.f,3.f);

	// Get a handle for our "myTextureSampler" uniform
	// GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");
	GLuint TextureID1 = loadPNG("Grass.png");
	GLuint TextureID2 = loadPNG("Dirt.png");
	GLuint TextureID3 = loadPNG("Rock.png");
	GLuint TextureID4 = loadPNG("Terrain_blendmap_1.png");
	GLuint TextureID5 = loadPNG("Grass1.png");

	GLuint TextureLoc1 = glGetUniformLocation(programID, "TextureSampler1");
	GLuint TextureLoc2 = glGetUniformLocation(programID, "TextureSampler2");
	GLuint TextureLoc3 = glGetUniformLocation(programID, "TextureSampler3");
	GLuint TextureLoc4 = glGetUniformLocation(programID, "BlendingTexture");
	GLuint TextureLoc5 = glGetUniformLocation(programID1, "TextureSampler4");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = loadOBJF33("cube.obj", vertices, uvs, normals);
	createquad(vertices, uvs);
	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		// glUseProgram(programID);
		glUseProgram(programID1);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::vec3 viewPos = computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID1, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(lightPos_location, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPos_location, viewPos.x, viewPos.y, viewPos.z);

		// Bind our texture in Texture Unit 0
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, Texture);
		// // Set our "myTextureSampler" sampler to use Texture Unit 0
		// glUniform1i(TextureID, 0);

		// Bind our texture in Texture Unit 1
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID1);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureLoc1, 0);

		// Bind our texture in Texture Unit 2
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, TextureID2);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureLoc2, 1);

		// Bind our texture in Texture Unit 3
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TextureID3);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureLoc3, 2);

		// Bind our texture in Texture Unit 4
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, TextureID4);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureLoc4, 3);

		// Bind our texture in Texture Unit 5
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, TextureID5);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureLoc5, 4);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	// glDeleteTextures(1, &Texture);
	glDeleteTextures(1, &TextureID1);
	glDeleteTextures(1, &TextureID2);
	glDeleteTextures(1, &TextureID3);
	glDeleteTextures(1, &TextureID4);
	glDeleteTextures(1, &TextureID5);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

