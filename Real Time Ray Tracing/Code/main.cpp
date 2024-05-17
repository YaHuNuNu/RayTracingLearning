#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<iostream>
#include "ModelLoader.h"
#include "ShaderHandle.h"
#include "hdrloader.h"
using namespace glm;

//���ڴ�С
int WIDTH = 800;
int HEIGHT = 800;

//�ӿڽǶ�
float FOV_Y = 45.0f;
float NEAR = 0.1f;
float FAR = 100.f;

//������ӽ�
float YAW = -90.0f;
float PITCH = 0.0f;

//�����λ��
vec3 cameraPos = vec3(0.0f, 8.0f, 15.0f);
vec3 cameraFront = vec3(0.0f, -1.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

//��һ֡��Ⱦʱ��
float frameDeltaTime = 0, lastTime = 0;
//֡������
int frameCounter = 0;

//ֱ�Ӹ���NDC�ռ��µ�Զƽ�������
std::vector<float> Screen = {
	-1,  1,  1,
	-1, -1,  1,
	 1, -1,  1,
	 1, -1,  1,
	 1,  1,  1,
	-1,  1,  1
};

//���ڻ����С�ı�ʱ�Ļص�����
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
//���ص�����
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
//���ֻص�����
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
//�����¼�
void InputHandle(GLFWwindow* window);

int main() {
	//��ʼ��GLFW	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//��������
	GLFWwindow* window = window = glfwCreateWindow(WIDTH, HEIGHT, "myWindow", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//���ع�꣬�������ָ��
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//ע�ᴰ�ڱ仯�ص�����
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	//ע�����ص�����
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	//��ʼ��GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

/*-------------------------------------------------------------------------------------------*/

	//������ɫ��
	ShaderHandle::Program renderShader("./Assets/Shader/Render.vert", "./Assets/Shader/Render.frag");
	ShaderHandle::Program showShader("./Assets/Shader/Show.vert", "./Assets/Shader/Show.frag");

	//����Screen����
	unsigned int ScreenVAO = ShaderHandle::LoadVertices(Screen);

	//����HDR��ͼ
	unsigned int hdrTexture = ShaderHandle::LoadHDR("./Assets/Textures/dikhololo_night_2k.hdr");
	renderShader.AddTexture("hdrTexture", hdrTexture, GL_TEXTURE_2D);

	//����ģ��,��ͨtexture������ݽ��д���������Ҫ��texture buffer������ԭʼ����
	ModelLoad::ModelLoaderResult loadedResult;
	ModelLoad::LoadByAssimp("./Assets/Models/Mesh000.ply", loadedResult);
	ModelLoad::LoadByAssimp("./Assets/Models/Mesh001.ply", loadedResult);
	glm::mat4 trans = glm::mat4(1.0f);
	//trans = glm::translate(trans, glm::vec3(0, 0, 0));
	trans = glm::scale(trans, glm::vec3(30.0f, 0.01f, 20.0f));
	ModelLoad::LoadByTinyObj("./Assets/Models/quad.obj", loadedResult, trans);
	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(8.0f, 5.0f, 0.0f));
	trans = glm::scale(trans, glm::vec3(3.0f, 3.0f, 3.0f));
	ModelLoad::LoadByTinyObj("./Assets/Models/sphere.obj", loadedResult, trans);

	ModelLoad::BuildAABB(loadedResult);
	//TTB:Triangle Texture Buffer; NTB:Node Texture Buffer
	unsigned int TTB = ShaderHandle::LoadTextureBuffer(loadedResult.triangles.size() * sizeof(ModelLoad::Triangle), &loadedResult.triangles[0]);
	unsigned int NTB = ShaderHandle::LoadTextureBuffer(loadedResult.nodeData.size() * sizeof(ModelLoad::AABBNode), &loadedResult.nodeData[0]);
	renderShader.AddTexture("TTB", TTB, GL_TEXTURE_BUFFER);
	renderShader.AddTexture("NTB", NTB, GL_TEXTURE_BUFFER);

	//֡����
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	unsigned int texColor, texNormal, texPosition;

	glGenTextures(1, &texColor);
	glBindTexture(GL_TEXTURE_2D, texColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &texNormal);
	glBindTexture(GL_TEXTURE_2D, texNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &texPosition);
	glBindTexture(GL_TEXTURE_2D, texPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColor, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texNormal, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texPosition, 0);
	if (!glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FRAMEBUFFER ERROR\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//renderShader.AddTexture("framebuffer", texColor, GL_TEXTURE_2D);
	showShader.AddTexture("texColor", texColor, GL_TEXTURE_2D);
	showShader.AddTexture("texNormal", texNormal, GL_TEXTURE_2D);
	showShader.AddTexture("texPosition", texPosition, GL_TEXTURE_2D);

	//�ر���Ȼ���
	glDisable(GL_DEPTH_TEST);
	// gamma����
	glEnable(GL_FRAMEBUFFER_SRGB);
	float timer = 0;
	int FPS = 0;
	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		//����ʱ��
		frameDeltaTime = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();
		frameCounter++;
		timer += frameDeltaTime;
		FPS++;
		if (timer > 1) {
			timer = 0;
			system("cls");
			std::cout << "֡����" << FPS << std::endl;
			FPS = 0;
		}

		//��������
		InputHandle(window);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//���ɱ任����
		mat4 projection = glm::perspective(glm::radians(FOV_Y), (float)WIDTH / (float)HEIGHT, NEAR, FAR);
		mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		//����
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		renderShader.use(ScreenVAO);
		//����VP�����
		mat4 inverse_VP = glm::inverse(view);
		renderShader.setMat("inverse_VP", inverse_VP);
		renderShader.setVec3f("cameraPos", cameraPos);
		renderShader.SetUInt("frameCounter", frameCounter);
		renderShader.setVec2f("WindowSize", vec2(WIDTH, HEIGHT));
		//��Ⱦ���໺��
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, drawBuffers);
		glDrawArrays(GL_TRIANGLES, 0, Screen.size() / 3);
		//�ָ���������
		GLenum windowBuffer[] = { GL_FRONT_LEFT };
		glDrawBuffers(1, windowBuffer);

		//��ʾ
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		showShader.use(ScreenVAO);
		showShader.setVec2f("WindowSize", vec2(WIDTH, HEIGHT));
		glDrawArrays(GL_TRIANGLES, 0, Screen.size() / 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//�رմ���
	glfwTerminate();
	return 0;
}
/*-------------------------------------------------------------------------------------------*/

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	WIDTH = width;
	HEIGHT = height;
	glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	static float lastMousePosX = xpos;
	static float lastMousePosY = ypos;

	float xOffset = (xpos - lastMousePosX) * 0.05;
	float yOffset = (lastMousePosY - ypos) * 0.05;
	lastMousePosX = xpos;
	lastMousePosY = ypos;

	YAW += xOffset;
	PITCH += yOffset;

	if (PITCH > 89.0f)
		PITCH = 89.0f;
	if (PITCH < -89.0f)
		PITCH = -89.0f;

	cameraFront.x = cos(glm::radians(YAW)) * cos(glm::radians(PITCH));
	cameraFront.y = sin(glm::radians(PITCH));
	cameraFront.z = sin(glm::radians(YAW)) * cos(glm::radians(PITCH));
	cameraFront = normalize(cameraFront);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (FOV_Y >= 1.0f && FOV_Y <= 45.0f)
		FOV_Y -= yoffset;
	if (FOV_Y <= 1.0f)
		FOV_Y = 1.0f;
	if (FOV_Y >= 45.0f)
		FOV_Y = 45.0f;
}

void InputHandle(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//������ƶ�����û֡��Ⱦʱ�������ƶ���������֤���ٲ���֡��Ӱ��
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraFront * (frameDeltaTime * 3);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraFront * (frameDeltaTime * 3);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos += normalize(cross(cameraUp, cameraFront)) * (frameDeltaTime * 3);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += normalize(cross(cameraFront, cameraUp)) * (frameDeltaTime * 3);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		frameCounter = 0;

	//��Alt��ʾ���
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}