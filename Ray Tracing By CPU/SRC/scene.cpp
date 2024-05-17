#include<glm/glm.hpp>
#include "scene.h"

using glm::vec3;

std::vector<model*> models;

// 颜色
const vec3 RED(1, 0.0, 0.0);
const vec3 GREEN(0.0, 1, 0.0);
const vec3 BLUE(0.0, 0.0, 1);
const vec3 YELLOW(1.0, 1.0, 0.0);
const vec3 CYAN(0.1, 1.0, 1.0);
const vec3 MAGENTA(1.0, 0.1, 1.0);
const vec3 GRAY(0.5, 0.5, 0.5);
const vec3 WHITE(1.0, 1.0, 1.0);
//光强
const vec3 LIGHT(2.5);

//载入三角形时，保证输入点的顺序是顺时针
void loadScene()
{
	model* temp = nullptr;
	//方形灯,灯一定先创建
	temp = new triangle(vec3(-0.4, 0.99, 0.4), vec3(0.4, 0.99, 0.4), vec3(0.4, 0.99, -0.4), Material(LIGHT, 1.0, 0.0, true));
	models.push_back(temp);
	temp = new triangle(vec3(0.4, 0.99, -0.4), vec3(-0.4, 0.99, -0.4), vec3(-0.4, 0.99, 0.4), Material(LIGHT, 1.0, 0.0, true));
	models.push_back(temp);

	//球
	temp = new sphere(glm::vec3(-0.7, -0.7, -0.7), 0.3, Material(WHITE, 0.1, 0.8));
	models.push_back(temp);
	temp = new sphere(glm::vec3(0.0, -0.3, 0.0), 0.4, Material(YELLOW, 0.4, 0.7));
	models.push_back(temp);
	temp = new sphere(glm::vec3(0.7, 0.7, -0.7), 0.3, Material(GRAY, 0.9, 0.2));
	models.push_back(temp);


	//盒子
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
