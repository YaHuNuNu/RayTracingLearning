#include<glm/glm.hpp>
#include "scene.h"

using glm::vec3;

std::vector<model*> models;

// ��ɫ
const vec3 RED(1, 0.0, 0.0);
const vec3 GREEN(0.0, 1, 0.0);
const vec3 BLUE(0.0, 0.0, 1);
const vec3 YELLOW(1.0, 1.0, 0.0);
const vec3 CYAN(0.1, 1.0, 1.0);
const vec3 MAGENTA(1.0, 0.1, 1.0);
const vec3 GRAY(0.5, 0.5, 0.5);
const vec3 WHITE(1.0, 1.0, 1.0);
//��ǿ
const vec3 LIGHT(2.5);

//����������ʱ����֤������˳����˳ʱ��
void loadScene()
{
	model* temp = nullptr;
	//���ε�,��һ���ȴ���
	temp = new triangle(vec3(-0.4, 0.99, 0.4), vec3(0.4, 0.99, 0.4), vec3(0.4, 0.99, -0.4), Material(LIGHT, 1.0, 0.0, true));
	models.push_back(temp);
	temp = new triangle(vec3(0.4, 0.99, -0.4), vec3(-0.4, 0.99, -0.4), vec3(-0.4, 0.99, 0.4), Material(LIGHT, 1.0, 0.0, true));
	models.push_back(temp);

	//��
	temp = new sphere(glm::vec3(-0.7, -0.7, -0.7), 0.3, Material(WHITE, 0.1, 0.8));
	models.push_back(temp);
	temp = new sphere(glm::vec3(0.0, -0.3, 0.0), 0.4, Material(YELLOW, 0.4, 0.7));
	models.push_back(temp);
	temp = new sphere(glm::vec3(0.7, 0.7, -0.7), 0.3, Material(GRAY, 0.9, 0.2));
	models.push_back(temp);


	//����
	//bottom
	temp = new triangle(vec3(-1, -1, -1), vec3(1, -1, -1), vec3(1, -1, 1), Material(WHITE));
	models.push_back(temp);
	temp = new triangle(vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, -1, -1), Material(WHITE));
	models.push_back(temp);
	//top
	temp = new triangle(vec3(-1, 1, 1), vec3(1, 1, 1), vec3(1, 1, -1), Material(WHITE));
	models.push_back(temp);
	temp = new triangle(vec3(1, 1, -1), vec3(-1, 1, -1), vec3(-1, 1, 1), Material(WHITE));
	models.push_back(temp);
	//back
	temp = new triangle(vec3(-1, 1, -1), vec3(1, 1, -1), vec3(1, -1, -1), Material(CYAN));
	models.push_back(temp);
	temp = new triangle(vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1), Material(CYAN));
	models.push_back(temp);                                                                       
	//left
	temp = new triangle(vec3(-1, 1, 1), vec3(-1, 1, -1), vec3(-1, -1, -1), Material(BLUE));
	models.push_back(temp);
	temp = new triangle(vec3(-1, -1, -1), vec3(-1, -1, 1), vec3(-1, 1, 1), Material(BLUE));
	models.push_back(temp);
	//right
	temp = new triangle(vec3(1, 1, -1), vec3(1, 1, 1), vec3(1, -1, 1), Material(RED));
	models.push_back(temp);
	temp = new triangle(vec3(1, -1, 1), vec3(1, -1, -1), vec3(1, 1, -1), Material(RED));
	models.push_back(temp);
}

void clearScene()
{
	for (int i = 0; i < models.size(); i++)
	{
		delete models[i];
	}
}
