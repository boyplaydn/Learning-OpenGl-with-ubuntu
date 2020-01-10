// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>

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
struct Light {
	glm::vec3 position;
	glm::vec3 color;
};

struct Model{
	GLuint modelID;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};
struct Object {
	GLuint modelID;
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
	GLuint programID;
	GLuint number_Textures;
	GLuint number_CubeTextures;
	GLuint TextureIDs[4];
	GLuint CubeTextureIDs[4];
	GLuint lightID;
    GLuint materialID;
	GLuint VerTexArrayID;
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	bool selected;
	float rotated;
	bool isTransparent;
};
struct Material {
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    GLfloat shininess;
};

GLuint selected_programIDs;

void createquad(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals)
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

	glm::vec2 uv1 = glm::vec2(0.0f, 0.0f);
	uvs.push_back(uv1);
	glm::vec2 uv2 = glm::vec2(1.0f, 0.0f);
	uvs.push_back(uv2);
	glm::vec2 uv3 = glm::vec2(1.0f, 1.0f);
	uvs.push_back(uv3);
	glm::vec2 uv4 = glm::vec2(0.0f, 0.0f);
	uvs.push_back(uv4);
	glm::vec2 uv5 = glm::vec2(1.0f, 1.0f);
	uvs.push_back(uv5);
	glm::vec2 uv6 = glm::vec2(0.0f, 1.0f);
	uvs.push_back(uv6);

	glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
	normals.push_back(normal);
	normals.push_back(normal);
	normals.push_back(normal);
	normals.push_back(normal);
	normals.push_back(normal);
	normals.push_back(normal);

}

void bigcreatequad(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals)
{

}

glm::mat4 calCulateModelMatrix (Object object)
{
	// Transform object 1 to World Space
	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), object.position);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), object.scale);
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(object.rotated), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4  ModelMatrix = translateMatrix * rotationMatrix * scaleMatrix;
	return ModelMatrix;
}
void draw_object(Object object, Model model_object, Light light_object, GLuint programID, Material material_object, float time)
{
	// Use our shader
	glUseProgram(programID);
	// create VAO
	glGenVertexArrays(1, &object.VerTexArrayID);
	glBindVertexArray(object.VerTexArrayID);

	glGenBuffers(1, &object.vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, object.vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, model_object.vertices.size() * sizeof(glm::vec3), &model_object.vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &object.uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, object.uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, model_object.uvs.size() * sizeof(glm::vec2), &model_object.uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &object.normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, object.normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, model_object.normals.size() * sizeof(glm::vec3), &model_object.normals[0], GL_STATIC_DRAW);

	// Get a handle for our uniform
	GLuint mpvLoc = glGetUniformLocation(programID, "MVP");
	GLuint ModelMatrixLoc = glGetUniformLocation(programID, "ModelMatrix");
	GLuint ViewPosLoc = glGetUniformLocation(programID, "viewPos_worldspace");
	GLuint lightPosLoc = glGetUniformLocation(programID, "light.position");
	GLuint lightColorLoc = glGetUniformLocation(programID, "light.color");
	GLuint timeLoc = glGetUniformLocation(programID, "u_time");

	GLuint TextureLoc [8];
	// for (GLuint i = 0 ; i < object.number_Textures; i++)
	// {
	// 	TextureLoc[i] = glGetUniformLocation(programID, "texture" + i);
	// }
	TextureLoc[0] = glGetUniformLocation(programID, "fire_texture");
	TextureLoc[1] = glGetUniformLocation(programID, "disp_texture");
	TextureLoc[2] = glGetUniformLocation(programID, "alpha_texture");
    //call material
    GLuint material_ambient_Loc = glGetUniformLocation(programID, "material.ambientColor");
    GLuint material_diffuse_Loc = glGetUniformLocation(programID, "material.diffuseColor");
    GLuint material_specular_Loc = glGetUniformLocation(programID, "material.specularColor");
    GLuint material_shininess_Loc = glGetUniformLocation(programID, "material.shininess");
	// Compute the MVP matrix from keyboard and mouse input;
    glm::vec3 ViewPos = computeMatricesFromInputs();
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	ModelMatrix = calCulateModelMatrix(object);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(mpvLoc, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixLoc, 1, GL_FALSE, &ModelMatrix[0][0]);

	// Bind our texture in Texture Unit 
	// if (object.number_Textures > 0)
	// {
	// 	for (GLuint i = 0; i < object.number_Textures; i++)
	// 	{
	// 		glActiveTexture(GL_TEXTURE0 + i);
	// 		glBindTexture(GL_TEXTURE_2D, object.TextureIDs[i]);
	// 		glUniform1i(TextureLoc[i], GL_TEXTURE0 + i);

	// 	}
	// }
	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object.TextureIDs[0]);
	glUniform1i(TextureLoc[0], 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, object.TextureIDs[1]);
	glUniform1i(TextureLoc[1], 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, object.TextureIDs[2]);
	glUniform1i(TextureLoc[2], 2);

	// lightPos
	glUniform3f(lightPosLoc, light_object.position.x, light_object.position.y, light_object.position.z);
	glUniform3f(lightColorLoc, light_object.color.x, light_object.color.y, light_object.color.z);
    // materialPos
    glUniform3f(material_ambient_Loc, material_object.ambientColor.x, material_object.ambientColor.y, material_object.ambientColor.z);
    glUniform3f(material_diffuse_Loc, material_object.diffuseColor.x, material_object.diffuseColor.y, material_object.diffuseColor.z);
    glUniform3f(material_specular_Loc, material_object.specularColor.x, material_object.specularColor.y, material_object.specularColor.z);
    glUniform1f(material_shininess_Loc, material_object.shininess);
    
	// viewPos
	glUniform3f(ViewPosLoc, ViewPos.x, ViewPos.y, ViewPos.z);

	glUniform1f(timeLoc, time);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, object.vertexbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, object.uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	// 3nd 
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, object.normalbuffer);
	glVertexAttribPointer(
		2,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_TRUE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);


	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, model_object.vertices.size());
	// Draw border
	if (object.selected == true) {
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(selected_programIDs);
		GLuint mpvLoc = glGetUniformLocation(selected_programIDs, "MVP");
		float scale = 1.1;
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scale, scale, scale));
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(mpvLoc, 1, GL_FALSE, &MVP[0][0]);
		//1st attribute buffer :: vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, object.vertexbuffer);
		glVertexAttribPointer(
			0, // attribute
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized
			0, // stride
			(void*)0 // array buffer offset
		);
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, model_object.vertices.size());
		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}
void delete_object(Object object)
{

	glDeleteBuffers(1, &object.vertexbuffer);
	glDeleteBuffers(1, &object.uvbuffer);
	glDeleteVertexArrays(1, &object.VerTexArrayID);

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
	static double lastTime = glfwGetTime();
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
	// Enable stencil test
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	float time = 0;

	// Load model
	GLuint number_models = 3;
	Model models[3];
	// bool res = loadOBJF34("Moon 2K.obj", models[0].vertices, models[0].uvs, models[0].normals);
	// bool res = loadOBJF33("cube.obj", models[0].vertices, models[0].uvs, models[0].normals);
	createquad(models[0].vertices, models[0].uvs, models[0].normals);
	bigcreatequad(models[1].vertices, models[1].uvs, models[1].normals);

	//load cube
	int number_CubeTextures = 1;
	char * faces[6];
	faces[0] = "right.png";
	faces[1] = "left.png";
	faces[2] = "top.png";
	faces[3] = "bottom.png";
	faces[4] = "back.png";
	faces[5] = "front.png";

	GLuint CubeTextureIDs[2];
	CubeTextureIDs[0] = loadPNG_CubeMap(faces);
	// Create and compile our GLSL program from the shaders
	GLuint number_programIDs = 1;
	GLuint programIDs[2];
	programIDs[0] = LoadShaders("CubeMap.vertexshader", "CubeMap.fragmentshader");
	selected_programIDs = LoadShaders("Border.vertexshader", "Border.fragmentshader");

	// Load Textures
	int number_Textures = 4;
	GLuint TextureIDs[7];
	TextureIDs[0] = loadPNG("Grass1.png",1);
	TextureIDs[1] = loadPNG("DisplacementMap.png",2);
	TextureIDs[2] = loadPNG("ground.png",1);
	TextureIDs[3] = loadPNG("rose.png",1);
	//TextureIDs[1] = loadPNG("DisplacementMap.png",2);
	TextureIDs[4] = loadPNG("fire_mask_1.png",1);
	TextureIDs[5] = loadPNG("Window.png",0);

	//Light position
	GLuint number_Light = 1;
	Light lights[2];
	lights[0].color = glm::vec3(1.0f, 1.0f, 1.0f);
	lights[0].position = glm::vec3(3.0f, 3.0f, 3.0f);

    //load Material
    Material materials[3];
    //---------------------Material 1-------------------------------------
    materials[0].ambientColor = glm::vec3(0.0215f, 0.1745f, 0.0215f);
    materials[0].diffuseColor = glm::vec3(0.07568f, 0.61424f, 0.07568f);
    materials[0].specularColor = glm::vec3(0.633f, 0.727811f, 0.633f);
    materials[0].shininess = 0.6f*128.0;
    //---------------------Material 2-------------------------------------
    materials[1].ambientColor = glm::vec3(0.024725f, 0.1995f, 0.0745f);
    materials[1].diffuseColor = glm::vec3(0.075164f, 0.60648f, 0.2264f);
    materials[1].specularColor = glm::vec3(0.628281f, 0.555802f, 0.366065f);
    materials[1].shininess = 0.4f*128.0;
    //---------------------Material 3-------------------------------------
    materials[2].ambientColor = glm::vec3(0.24725f, 0.1995f, 0.0745f);
    materials[2].diffuseColor = glm::vec3(0.75164f, 0.60648f, 0.22648f);
    materials[2].specularColor = glm::vec3(0.628281f, 0.555802f, 0.366065f);
    materials[2].shininess = 0.4f*128.0;
	
	GLuint number_Object = 9;
	Object objects [10];
	//---------------------Object 1-------------------------------------
	objects[0].position = glm::vec3(0.0f, 0.3f, 0.5f);
	objects[0].scale = glm::vec3(0.2f);
	objects[0].rotation = glm::vec3(1.0f);
	objects[0].programID = 0;
	objects[0].modelID = 0;
	objects[0].number_Textures = 1;
	objects[0].number_CubeTextures = 0;
	objects[0].CubeTextureIDs[0] = 0;
	objects[0].lightID = 0;
    objects[0].materialID = 0;
	objects[0].TextureIDs[0] = TextureIDs[0];
	// objects[0].TextureIDs[1] = TextureIDs[1];
	// objects[0].TextureIDs[2] = TextureIDs[2];
	objects[0].selected = false;
	objects[0].isTransparent = false;
	objects[0].rotated = 10.0f;
	//---------------------Object 2-------------------------------------
	objects[1].position = glm::vec3(-0.5f, 0.3f, 0.5f);
	objects[1].scale = glm::vec3(0.2f);
	objects[1].rotation = glm::vec3(1.0f);
	objects[1].programID = 0;
	objects[1].modelID = 0;
	objects[1].number_Textures = 1;
	objects[1].lightID = 0;
    objects[1].materialID = 1;
	objects[1].TextureIDs[0] = TextureIDs[0];
	objects[1].selected = false;
	objects[1].isTransparent = false;
	objects[1].rotated = 10.0f;
	//---------------------Object 2.5-------------------------------------
	objects[3].position = glm::vec3(0.0f, 0.0f, 0.0f);
	objects[3].scale = glm::vec3(1.0f, 1.0f, 1.0f);
	objects[3].rotation = glm::vec3(1.0f);
	objects[3].programID = 0;
	objects[3].modelID = 0;
	objects[3].number_Textures = 1;
	objects[3].lightID = 0;
    objects[3].materialID = 1;
	objects[3].TextureIDs[0] = TextureIDs[2];
	objects[3].selected = false;
	objects[3].isTransparent = false;
	objects[3].rotated = -75.f;
    //---------------------Object 3-------------------------------------
	objects[2].position = glm::vec3(0.4f, 0.0f, 2.0f);
	objects[2].scale = glm::vec3(0.15f);
	objects[2].rotation = glm::vec3(1.0f);
	objects[2].programID = 0;
	objects[2].modelID = 0;
	objects[2].number_Textures = 1;
	objects[2].lightID = 0;
    objects[2].materialID = 2;
	objects[2].TextureIDs[0] = TextureIDs[0];
	objects[2].selected = false;
	objects[2].isTransparent = false;
	objects[2].rotated = 10.0f;

	objects[4].position = glm::vec3(-0.2f, 0.2f, 1.2f);
	objects[4].scale = glm::vec3(0.15f);
	objects[4].rotation = glm::vec3(1.0f);
	objects[4].programID = 0;
	objects[4].modelID = 0;
	objects[4].number_Textures = 1;
	objects[4].lightID = 0;
    objects[4].materialID = 2;
	objects[4].TextureIDs[0] = TextureIDs[0];
	objects[4].selected = false;
	objects[4].isTransparent = false;
	objects[4].rotated = 10.0f;

	objects[5].position = glm::vec3(0.2f, 0.2f, 1.2f);
	objects[5].scale = glm::vec3(0.15f);
	objects[5].rotation = glm::vec3(1.0f);
	objects[5].programID = 0;
	objects[5].modelID = 0;
	objects[5].number_Textures = 1;
	objects[5].lightID = 0;
    objects[5].materialID = 2;
	objects[5].TextureIDs[0] = TextureIDs[0];
	objects[5].selected = false;
	objects[5].isTransparent = false;
	objects[5].rotated = 10.0f;

	//---------------------Object 1-------------------------------------
	objects[6].position = glm::vec3(0.0f, -0.1f, 2.0f);
	objects[6].scale = glm::vec3(0.1f);
	objects[6].rotation = glm::vec3(1.0f);
	objects[6].programID = 0;
	objects[6].modelID = 0;
	objects[6].number_Textures = 1;
	objects[6].number_CubeTextures = 0;
	objects[6].CubeTextureIDs[0] = 0;
	objects[6].lightID = 0;
    objects[6].materialID = 0;
	objects[6].TextureIDs[0] = TextureIDs[3];
	objects[6].selected = false;
	objects[6].isTransparent = true;
	objects[6].rotated = 0.0f;

	//---------------------Object 1-------------------------------------
	objects[7].position = glm::vec3(-0.4f, 0.0f, 0.6f);
	objects[7].scale = glm::vec3(0.1f);
	objects[7].rotation = glm::vec3(1.0f);
	objects[7].programID = 0;
	objects[7].modelID = 0;
	objects[7].number_Textures = 1;
	objects[7].number_CubeTextures = 0;
	objects[7].CubeTextureIDs[0] = 0;
	objects[7].lightID = 0;
    objects[7].materialID = 0;
	objects[7].TextureIDs[0] = TextureIDs[3];
	objects[7].selected = false;
	objects[7].isTransparent = false;
	objects[7].rotated = 0.0f;

	//---------------------Object 1-------------------------------------
	objects[8].position = glm::vec3(0.6f, -0.0f, 0.6f);
	objects[8].scale = glm::vec3(0.1f);
	objects[8].rotation = glm::vec3(1.0f);
	objects[8].programID = 0;
	objects[8].modelID = 0;
	objects[8].number_Textures = 1;
	objects[8].number_CubeTextures = 0;
	objects[8].CubeTextureIDs[0] = 0;
	objects[8].lightID = 0;
    objects[8].materialID = 0;
	objects[8].TextureIDs[0] = TextureIDs[3];
	objects[8].selected = false;
	objects[8].isTransparent = false;
	objects[8].rotated = 0.0f;
	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		//1st render pass draw objects as normal, writing to the stencil buffer
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);

		time += deltaTime;
		std::map<float, Object> sorted;
		
		for (GLuint i = 0; i < number_Object; i++)
		{	
			if(objects[i].isTransparent == false)
			{
				Model model_object = models[objects[i].modelID];
				Light light_object = lights[objects[i].lightID];
				GLuint programID = programIDs[objects[i].programID];
				Material material_object = materials[objects[i].materialID];
				draw_object(objects[i], model_object, light_object, programID, material_object, time);
			}
			else
			{
				glm::vec3 CameraPos = computeMatricesFromInputs();
				float distance = glm::length(CameraPos - objects[i].position);
				sorted[distance] = objects[i];
			}
		}
		for (std::map<float, Object>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			Object transparentObject = it->second;
			Model model_object = models[transparentObject.modelID];
			Light light_object = lights[transparentObject.lightID];
			Material material_object = materials[transparentObject.materialID];
			GLuint programID = programIDs[transparentObject.programID];
			draw_object(transparentObject, model_object, light_object, programID, material_object, time);
		}
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		lastTime = currentTime;

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );
	
	// Cleanup VBO and shader

	for (int i= 0; i < number_Textures; i++)
	{
		glDeleteTextures(1, &TextureIDs[i]);
	}
	// for (int i= 0; i < number_CubeTextures; i++)
	// {
	// 	glDeleteTextures(1, &CubeTextureIDs[i]);
	// }
	for (int i= 0; i < number_programIDs; i++)
	{
		glDeleteProgram(programIDs[i]);
	}

	for (int i= 0; i < number_Object; i++)
	{
		delete_object(objects[i]);
	}
	// Cleanup VBO and shader
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

