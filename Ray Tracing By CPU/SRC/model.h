#pragma once

#include<glm/glm.hpp>
#include<vector>

//光线
struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};

//模型材质
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


//多种模型的统一接口
class model
{
public:
	Material material;

	model(Material _material) : material(_material) {}
	virtual ~model() {}

	/*光线与该模型求交，若相交，则将 P = o + t * d 的t返回，且将交点的材质信息通过引用返回
	若不相交则返回0。 ps:若光射线在起点在模型上，函数会判断相交并返回0，但认为我们此时不相交*/
	virtual float ray_intersect(const Ray& ray)const = 0;
	virtual Material getMaterial(glm::vec3 intersection)const = 0;
};

//球模型
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

		//计算三角形法向量(摆放三角形时，请保证输入三角形的点为顺时针)
		this->material.normal = glm::normalize(glm::cross(point_3 - point_1, point_2 - point_1));
	}

	~triangle() {}

	float ray_intersect(const Ray& ray)const override;
	virtual Material getMaterial(glm::vec3 intersection)const override;
};