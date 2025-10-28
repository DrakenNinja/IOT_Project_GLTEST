#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#include <stdio.h>
#include <stdlib.h>

//#include <GL/glew.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>


using namespace glm;

GLuint LoadShaders(const char* frag_shader_path, const char* vert_shader_path);
void framebuffer_size_CB(GLFWwindow* window, int width, int height);

int main(){
	if( !glfwInit() ){
    fprintf( stderr, "Failed to initialize GLFW\n" );
    return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 1); // 1x antialiasing

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Open a window and create its OpenGL context
	GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)
	window = glfwCreateWindow( 720, 720, "Tutorial 01", NULL, NULL);

	if( window == NULL ){
			fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
			glfwTerminate();
			return -1;
	}

	glfwMakeContextCurrent(window); // Initialize GLEW

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
			printf("Failed to initialize GLAD\n");
			return -1;
	}   
	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_CB);



	//setting up triangle
	static const GLfloat triangle[] ={
		-1.0f, -1.0f, 0.0f,
		 1.0f, -0.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};


/*	GLfloat quad[] = {
		//x,  y, 	z
		 1.0f,  1.0f, 0.0f,  // top right
     1.0f,  0.0f, 0.0f,  // bottom right
     0.0f,  0.0f, 0.0f,  // bottom left
     0.0f,  1.0f, 0.0f   // top left 
	};*/


	GLfloat textured_quad[] = {
    // positions          // colors           // texture coords
     1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     1.0f, 	-1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	   -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
     -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,    0.0f, 1.0f    // top left 
};
	
	unsigned int indices[]{
		0, 1, 3,
		1, 2, 3
	};
	

	GLuint programID = LoadShaders("fragshader.gl", "vertshader.gl"); 

	//vertex Buffer Object, Vertex Atribute object
	unsigned int VBO, VAO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textured_quad), textured_quad, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);  

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));

	glEnableVertexAttribArray(2);  



	//load image
	int width, height, nrCannels;
	stbi_set_flip_vertically_on_load(true);  

	unsigned char *data = stbi_load("bevershade.png", &width, &height, &nrCannels, 3);
	//unsigned char *data = stbi_load("bever.png", &width, &height, &nrCannels, 3);
//	unsigned char *data = stbi_load("shade_sim.png", &width, &height, &nrCannels, 3);
	if(!data){
		printf("img load fail");
		return 1;
	}

	
	//texture generating
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_TEXTURE_BORDER_COLOR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_TEXTURE_BORDER_COLOR);	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);	


	while(!glfwWindowShouldClose(window)){
	
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //clear background color
    glClear(GL_COLOR_BUFFER_BIT); //clear bg with said color
		
		glBindTexture(GL_TEXTURE_2D, texture);	
		glUseProgram(programID); //use the compiles shaders
		glBindVertexArray(VAO); 

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0); 

		glfwSwapBuffers(window);
		glfwPollEvents();		
	}

	glfwTerminate();
	return 0;
}
//
//
//
//
void framebuffer_size_CB(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

//shader compiling/linking pipelie
GLuint LoadShaders(const char* frag_shader_path, const char* vert_shader_path){

	GLuint fragshaderID = glad_glCreateShader(GL_FRAGMENT_SHADER);
	GLuint vertshaderID = glad_glCreateShader(GL_VERTEX_SHADER);
	
	
	printf("loading\n");
	//loading in fragment shader from file
	std::string FragmentShaderCode;
	std::ifstream fragmentShaderStream(frag_shader_path, std::ios::in);
	if(fragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << fragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		fragmentShaderStream.close();	
	}

	std::string vertShaderCode;
	std::ifstream vertShaderStream(vert_shader_path, std::ios::in);
	if(vertShaderStream.is_open()){
		std::stringstream sstr;
		sstr << vertShaderStream.rdbuf();
		vertShaderCode = sstr.str();
		vertShaderStream.close();	
	}


	
	GLint result = GL_FALSE;
	int Infologlength;

	//compile frag shader	
	printf("compiling frag shader: %s\n", frag_shader_path);
	const char* fragsourcepointer = FragmentShaderCode.c_str();
	glShaderSource(fragshaderID, 1, &fragsourcepointer, NULL);
	glCompileShader(fragshaderID);


	
	//checking compiled shader
	printf("checking compiled shader\n");
	glGetShaderiv(fragshaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragshaderID, GL_INFO_LOG_LENGTH, &Infologlength);
	if(Infologlength > 0){
		std::vector<char> FragmentShaderErrorMessage(Infologlength + 1);
		glGetShaderInfoLog(fragshaderID, Infologlength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}


	printf("compiling vert shader: %s\n", vert_shader_path);
	const char* vertsourcepointer = vertShaderCode.c_str();
	glShaderSource(vertshaderID, 1, &vertsourcepointer, NULL);
	glCompileShader(vertshaderID);


	
	//checking compiled shader
	printf("checking compiled vert shader\n");
	glGetShaderiv(vertshaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertshaderID, GL_INFO_LOG_LENGTH, &Infologlength);
	if(Infologlength > 0){
		std::vector<char> vertShaderErrorMessage(Infologlength + 1);
		glGetShaderInfoLog(vertshaderID, Infologlength, NULL, &vertShaderErrorMessage[0]);
		printf("%s\n", &vertShaderErrorMessage[0]);
	}


	//linking shader
	printf("attatching\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, fragshaderID);
	glAttachShader(programID, vertshaderID);
	glLinkProgram(programID);


	//check program
	printf("checking program\n");
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &Infologlength);
	if(Infologlength > 0){
		std::vector<char> Programerrormsg(Infologlength+1);
		glGetProgramInfoLog(programID, Infologlength, NULL, &Programerrormsg[0]);
		printf("%s\n", &Programerrormsg[0]);

	}

	glDetachShader(programID, fragshaderID);
	glDetachShader(programID, vertshaderID);
	
	glDeleteShader(fragshaderID);
	glDeleteShader(vertshaderID);
	printf("shader done\n");
	return programID;
//	return 0;
}



















