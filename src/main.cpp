#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm\gtx\dual_quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../include/stb_image.h"
#include "../include/shader.h"
#include "../include/camera.h"
#include "../include/animator.h"
#include "../include/model_animation.h"
#include "../include/model_manager.h"
#include "../include/skybox.h"

#include "../include/bone.h"

//#include "../include/texture.h"

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

#include <iostream>

using namespace std;
using namespace glm;


const int WindowW = 800;
const int WindowH = 600;

Camera camera(vec3(0.0f, 0.0f, 0.3f));

float lastX = WindowW / 2;
float lastY = WindowH / 2;
bool firstMouse = 1;

float dt = 0;
float lastFrame = 0;

float ang = 0.;


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    	glfwSetWindowShouldClose(window, true);
    
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    	camera.ProcessKeyboard(FORWARD, dt);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    	camera.ProcessKeyboard(BACKWARD, dt);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
     	camera.ProcessKeyboard(LEFT, dt);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
     	camera.ProcessKeyboard(RIGHT, dt);
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, dt);
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, dt);  	
		
    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.acceleration = 0.01f;  			
    else if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
		camera.acceleration = 10.0f;
	else camera.acceleration = 1.0f;
	
		
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = 0;
	}
	float ox = xpos - lastX;
	float oy = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	
	camera.ProcessMouseMovement(ox, oy);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		ang += 45.; 
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WindowW, WindowH, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

//    stbi_set_flip_vertically_on_load(true);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

/*light cube (sun) init		begin*/

    float vertices[] = {
         // координаты        // нормали           // текстурные координаты
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

	unsigned int VBO, VAO;;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	

unsigned int cubeVAO;
glGenVertexArrays(1, &cubeVAO);
glGenBuffers(1, &VBO);

glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

glBindVertexArray(cubeVAO);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
glEnableVertexAttribArray(2);

unsigned int diffuseMap = loadTexture("./resources/img/container.png");

/*end*/


    // build and compile shaders
    Shader shaderMDLA("shaders/ModelA/modelA.vs", "shaders/ModelA/model2.fs");
    Shader shaderMDLS("shaders/Model/modelS.vs", "shaders/Model/model2.fs");
	Shader shaderSKY("shaders/SkyBox/skybox.vs", "shaders/SkyBox/skybox.fs");
	
	Shader lightCubeShader("shaders/4.1.light_cube.vs", "shaders/4.1.light_cube.fs");
	shaderMDLS.use();
	shaderMDLS.setInt("material.diffuse", 0);
	
	shaderMDLA.use();
	shaderMDLA.setInt("material.diffuse", 0);
    // load models	  
    const char* AnimationsList[]
    {
    	"resources/3D/tetris/characters2/animations/figureIDLE0.dae",
    	"resources/3D/tetris/characters2/animations/figureIDLE1.dae",
    	"resources/3D/tetris/characters2/animations/run.dae",
    	"resources/3D/tetris/characters2/animations/jump.dae",
    	"resources/3D/tetris/characters2/animations/hammerIDLE0.dae",
    	"resources/3D/tetris/characters2/animations/hammerIDLE1.dae",
    	"resources/3D/tetris/characters2/animations/blow.dae",
	};
    
//	MDLManager MDL1(DYNAMIC, shaderMDLA, "resources/3D/tetris/characters/Ellipsoid/model.dae", "resources/3D/tetris/characters/Ellipsoid/idle.dae");
//	MDLManager MDL1(DYNAMIC, shaderMDLA, "resources/3D/tetris/characters2/models/ellipsoidC.dae", "resources/3D/tetris/characters2/animations/ellipsoidIDLE0.dae");
	MDLManager MDL1(DYNAMIC, shaderMDLA, "resources/3D/tetris/characters/S/model.dae", "resources/3D/tetris/characters/S/idle.dae");
	MDLManager MDL2(STATIC, shaderMDLS, "resources/3D/tetris/figures/O/model.dae");
//	MDLManager MDL2(STATIC, shaderMDLS, "resources/3D/tetris/primitives/model.dae");

//	MDLManager MDL2(STATIC, shaderMDLS, "resources/3D/test/bob/boblampclean.md5mesh");
//	MDLManager MDL1(DYNAMIC, shaderMDLA, "resources/3D/test/bob/boblampclean.md5mesh", "resources/3D/test/bob/boblampclean.md5anim");
	
	SkyBox SKY(shaderSKY, "resources/img/sky2", 10000.0f);


    float totalFPS = 0;
    int frame = 0;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInput(window);
        
        float currentFrame = glfwGetTime();
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;
        totalFPS += 1/dt;
    	frame++;
//		printf("FPS: %.1f\tav: %.1f\tframes: %d\n", 1/dt, totalFPS/frame, frame);
//		printf("Yaw: %f\n", camera.Yaw);
//		printf("X: %.2f\tY: %.2f\tZ: %.2f\n", camera.Position.x, camera.Position.y, camera.Position.z);

//		cout << ang << "\n";

        /*render*/
		MDL1.Animate(dt);

		mat4 model = mat4(1.0);
        mat4 view = camera.GetViewMatrix();
        mat4 projection = perspective(radians(camera.Zoom), (float)WindowW / (float)WindowH, 0.1f, 100.0f);
		
		vec3 lightPos(.0f, 2.0f, -1.0f);

		SKY.SetViewMat(view);
		SKY.SetProjMat(projection);
		SKY.Draw();
		
		
//	cout << "\n\nview matrix\n";
//	printf("%.0f %.0f %.0f %.0f\n", view[0].x, view[0].y, view[0].z, view[0].w);
//	printf("%.0f %.0f %.0f %.0f\n", view[1].x, view[1].y, view[1].z, view[1].w);
//	printf("%.0f %.0f %.0f %.0f\n", view[2].x, view[2].y, view[2].z, view[2].w);
//	printf("%.0f %.0f %.0f %.0f\n", view[3].x, view[3].y, view[3].z, view[3].w);


		shaderMDLA.use();
		shaderMDLA.setVec3("light.position", lightPos);
		shaderMDLA.setVec3("viewPos", camera.Position);
		
		shaderMDLA.setVec3("light.ambient", .2f, .2f, .2f);
		shaderMDLA.setVec3("light.diffuse", .5f, .5f, .5f);
		shaderMDLA.setVec3("light.specular", .5f, .5f, .5f);
		
		shaderMDLA.setVec3("material.specular", .5f, .5f, .5f);
		shaderMDLA.setFloat("material.shininess", 512.f);

mat4 T;
	T[0].x = -1; T[0].y = 0; T[0].z = 0; T[0].w = 0;
	T[1].x = 0; T[1].y = 0; T[1].z = 1; T[1].w = 0;
	T[2].x = 0; T[2].y = 1; T[2].z = 0; T[2].w = 0;
	T[3].x = 0; T[3].y = 0; T[3].z = 0; T[3].w = 1;	
	
		
		MDL1.SetModelMat(model);
		MDL1.SetViewMat(view);
		MDL1.SetProjMat(projection);
		MDL1.Translate(1.);
//		MDL1.Scale(vec3(0.2, 0.2, -0.2));
//		MDL1.Rotate(ang, vec3(0, 0, 1));
//		MDL1.Rotate(90, vec3(0, 1, 0));
		MDL1.Scale(0.03);
		MDL1.Draw();


		shaderMDLS.use();
		shaderMDLS.setVec3("light.position", lightPos);
		shaderMDLS.setVec3("viewPos", camera.Position);
		
		shaderMDLS.setVec3("light.ambient", .2f, .2f, .2f);
		shaderMDLS.setVec3("light.diffuse", .5f, .5f, .5f);
		shaderMDLS.setVec3("light.specular", .5f, .5f, .5f);
		
		shaderMDLS.setVec3("material.specular", .5f, .5f, .5f);
		shaderMDLS.setFloat("material.shininess", 64.f);
		
		shaderMDLS.setMat4("projection", projection);
		shaderMDLS.setMat4("view", view);
		shaderMDLS.setMat4("model", model);	    
		
		MDL2.SetModelMat(model);
		MDL2.SetViewMat(view);
		MDL2.SetProjMat(projection);
		MDL2.Translate(vec3(-1, 0, 0));
		MDL2.Rotate(ang, vec3(1, 0, 0));
//		MDL2.Rotate(90, vec3(0, 1, 0));
		MDL2.Scale(vec3(0.5, 0.5, 0.5));
		MDL2.Draw();

glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, diffuseMap);
glBindVertexArray(cubeVAO);
//glDrawArrays(GL_TRIANGLES, 0, 36);

/*light cube (sun) draw		begin*/
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		model = mat4(1.0f);
		model = translate(model, lightPos);
		model = scale(model, vec3(.1f)); // куб меньшего размера
		lightCubeShader.setMat4("model", model);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
/*end*/
		
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}







