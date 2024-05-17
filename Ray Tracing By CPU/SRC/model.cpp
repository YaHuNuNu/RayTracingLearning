#include "model.h"

float sphere::ray_intersect(const Ray& ray)const
{	
	//������������ķ���: d^2 * t^2 + 2d(o - c)t + (o - c)^2 - r^2 = 0 ���
	glm::vec3 c_o = ray.origin - center;
	float b = glm::dot(ray.direction, c_o) * 2.0;
	float c = glm::dot(c_o, c_o) - radius * radius;
	float d = b * b - 4 * c;
	if (d < 0.0) return 0.0;

	float sqrt_d = sqrt(d);
	float t1 = (-1.0 * b - sqrt_d) / 2.0;
	float t2 = (-1.0 * b + sqrt_d) / 2.0;

	float t = 0.0;
	if (t1 > 0.0) t = t1;
	else if (t2 > 0.0) t = t2;

	return t;
}

float triangle::ray_intersect(const Ray& ray)const
{
	//��ͳ�������������㷨
	/*
	//����ƽ�淽���ж������Ƿ�������������ƽ���ཻ�����ǣ����������
	glm::vec3 normal = this->material.normal;
	float d_n = glm::dot(ray.direction, normal);
	if (d_n == 0.0) return 0.0;

	float t = (glm::dot(normal, point_1) - glm::dot(ray.origin, normal)) / d_n;
	if (t <= 0.0) return 0.0;

	glm::vec3 P = ray.origin + t * ray.direction;

	//�жϽ����Ƿ�����������
	glm::vec3 point1_2 = point_2 - point_1;
	glm::vec3 point1_P = P - point_1;
	glm::vec3 crossValue_1 = glm::cross(point1_P, point1_2);

	glm::vec3 point2_3 = point_3 - point_2;
	glm::vec3 point2_P = P - point_2;
	glm::vec3 crossValue_2 = glm::cross(point2_P, point2_3);

	glm::vec3 point3_1 = point_1 - point_3;
	glm::vec3 point3_P = P - point_3;
	glm::vec3 crossValue_3 = glm::cross(point3_P, point3_1);

	float decided_value_1 = glm::dot(crossValue_1, crossValue_2);
	float decided_value_2 = glm::dot(crossValue_1, crossValue_3);
	if (decided_value_1 > 0.0 && decided_value_2 > 0.0) return t;

	return 0.0;
	*/

	//Moller-Trumbore�㷨
	glm::vec3 S = ray.origin - point[0];
	glm::vec3 E1 = point[1] - point[0];
	glm::vec3 E2 = point[2] - point[0];

	glm::vec3 DxE2 = glm::cross(ray.direction, E2);
	glm::vec3 SxE1 = glm::cross(S, E1);

	float parameter = 1.0 / glm::dot(DxE2, E1);

	float t = parameter * glm::dot(SxE1, E2);
	float u = parameter * glm::dot(DxE2, S);
	float v = parameter * glm::dot(SxE1, ray.direction);

	if (t > 0.0 && u >= 0.0 && v >= 0.0 && (u + v) <= 1.0) return t;
	else return 0.0;
}

//Բ����ʵʱ���㷨����
Material sphere::getMaterial(glm::vec3 intersection) const
{
	Material reMaterial = this->material;
	reMaterial.normal = glm::normalize(intersection - this->center);
	return reMaterial;
}

//������ֱ�ӷ��ط�����
Material triangle::getMaterial(glm::vec3 intersection) const
{
	return this->material;
}