#pragma once

#include<glm/glm.hpp>
#include<vector>

//����
struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};

//ģ�Ͳ���
struct Material {
	bool is_luminous;
	glm::vec3 albedo;
	glm::vec3 normal;
	float roughness;
	float metallic;

	Material() {}
	Material(glm::vec3 albe, float roug = 1.0, float meta = 0.0, bool isLu = false, glm::vec3 norm = glm::vec3(0.0))
		: albedo(albe), roughness(roug),metallic(meta), is_luminous(isLu), normal(norm) {}
};


//����ģ�͵�ͳһ�ӿ�
class model
{
public:
	Material material;

	model(Material _material) : material(_material) {}
	virtual ~model() {}

	/*�������ģ���󽻣����ཻ���� P = o + t * d ��t���أ��ҽ�����Ĳ�����Ϣͨ�����÷���
	�����ཻ�򷵻�0�� ps:���������������ģ���ϣ��������ж��ཻ������0������Ϊ���Ǵ�ʱ���ཻ*/
	virtual float ray_intersect(const Ray& ray)const = 0;
	virtual Material getMaterial(glm::vec3 intersection)const = 0;
};

//��ģ��
class sphere :public model
{
public:
	glm::vec3 center;
	float radius;


	sphere(glm::vec3 center, float radius, Material material)
		: model(material), center(center), radius(radius) {}

	~sphere() {};

	float ray_intersect(const Ray& ray)const override;
	virtual Material getMaterial(glm::vec3 intersection)const override;
};


class triangle : public model
{
public:
	glm::vec3 point[3];
	float area;

	triangle(glm::vec3 point_1, glm::vec3 point_2, glm::vec3 point_3, Material material)
		: model(material)
	{
		point[0] = point_1;
		point[1] = point_2;
		point[2] = point_3;

		area = 0.5 * glm::length(glm::cross(point_3 - point_1, point_2 - point_1));

		//���������η�����(�ڷ�������ʱ���뱣֤���������εĵ�Ϊ˳ʱ��)
		this->material.normal = glm::normalize(glm::cross(point_3 - point_1, point_2 - point_1));
	}

	~triangle() {}

	float ray_intersect(const Ray& ray)const override;
	virtual Material getMaterial(glm::vec3 intersection)const override;
};