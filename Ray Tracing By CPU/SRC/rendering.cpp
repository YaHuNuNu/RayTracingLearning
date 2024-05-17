#include <random>
#include "rendering.h"

#include<iostream>

const float PI = 3.1415926;
//��Ͳ������������һ�ε���Ĳ�����
const int NUM_SAMPLE = 1500;
//��Ӧ�޸Ĵ����ֵ
const int DEPTH = 3;
//ֱ�ӹ������ӹ��ձ���
const float T = 0.6;


std::random_device rd;//���������ɱȽϺķ�����
std::mt19937 gen(rd());
float random_float(float left /* = 0 */, float right/* = 1 */)
{
	std::uniform_real_distribution<float> dis(left, right);
	return dis(gen);
}


glm::vec3 random_vector(glm::vec3 normal)
{
	//ʹ��theta��phi�Ƕȱ�ʾ������������theta��phi�Ͼ��Ȳ���
	float theta = random_float(0.0, 2 * PI);
	float phi = random_float(0.0, PI);
	glm::vec3 random_vec(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));

	//����λ���ڵ��������ת������������������� PS:���������պ��෴ʱ�������Ϊ0
	return glm::normalize(glm::normalize(random_vec) + normal);
}


glm::vec2 Hammersley(uint32_t i, int num_sample) {
	uint32_t bits = (i << 16u) | (i >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	float rdi = float(bits) * 2.3283064365386963e-10;
	return { float(i) / float(num_sample), rdi };
}

//�����õ������������߿ռ��У�������Ҫ���뷨����N����TBN����
glm::vec3 ImportanceSampleGGX(glm::vec2 Xi, glm::vec3 N, float a)
{
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	glm::vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	//���ǵ�������N��(0, 0, 1)�غ�ʱ
	glm::vec3 up = abs(N.z) < 0.999 ? glm::vec3(0.0, 0.0, 1.0) : glm::vec3(1.0, 0.0, 0.0);
	glm::vec3 tangent = normalize(cross(up, N));
	glm::vec3 bitangent = cross(N, tangent);

	glm::vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return glm::normalize(sampleVec);
}


glm::vec3 fresnelSchlick(float HdotV, glm::vec3 F0)
{
	return F0 + ((float)1.0 - F0) * (float)pow(HdotV, 5.0);
}

float DistributionGGX(float NdotH, float a)
{
	float a2 = a * a;
	float temp = NdotH * NdotH * (a2 - 1) + 1;

	return a2 / (PI * temp * temp);
}

float GeometrySchlickGGX(float NdotX, float k)
{
	return NdotX / (NdotX * (1.0 - k) + k);
}

float GeometrySmith(float NdotV, float NdotL, float a)
{
	//ֱ�ӹ���ʱ��k = (a + 1)^2 / 8.0
	//float k = a * a / 2.0;
	float k = (a + 1) * (a + 1) / 8.0;
	return GeometrySchlickGGX(NdotV, k) * GeometrySchlickGGX(NdotL, k);
}

//BRDF��ֵ�ǿ��Գ���1�ģ����Ƿ����ʳ�������ǣ��ڷǳ��⻬�ľ����ϻ����Գ���1
glm::vec3 BRDF(glm::vec3 V, glm::vec3 L, glm::vec3 H, const Material& material)
{
	float HdotV = glm::dot(H, V);
	float NdotV = glm::dot(material.normal, V);
	float NdotL = glm::dot(material.normal, L);
	float NdotH = glm::dot(material.normal, H);
	float a = material.roughness * material.roughness;

	glm::vec3 P = material.albedo * (float)(1.0 - material.metallic);
	glm::vec3 F0 = glm::mix(glm::vec3(0.04), material.albedo, material.metallic);

	glm::vec3 F = fresnelSchlick(HdotV, F0);
	float G = GeometrySmith(NdotV, NdotL, a);
	float D = DistributionGGX(NdotH, a);

	glm::vec3 specular = F * G * D / (float)(4.0 * NdotV * NdotL);
	glm::vec3 diffuse = ((float)1.0 - F) * P / PI;

	return diffuse + specular; 
}


glm::vec3 ray_scene_intersect(const Ray& ray, const std::vector<model*>& models, Material& material)
{
	//t�����ֵ��ʼ��
	float t = FLT_MAX;
	model* hitModel = nullptr;
	for (int i = 0; i < models.size(); i++)
	{
		model* curr_model = models[i];
		float temp = curr_model->ray_intersect(ray);

		if (temp == 0.0) continue;

		if (temp < t)
		{
			t = temp;
			hitModel = curr_model;
		}
	}
	glm::vec3 intersection = glm::vec3(0.0);
	if (hitModel != nullptr)
	{
		intersection = ray.origin + t * ray.direction;
		material = hitModel->getMaterial(intersection);
	}

	return intersection;
}


/*
glm::vec3 rayTracing(const Ray& ray, const std::vector<model*>& models, int depth)
{
	//�����ݹ����,����һ��������ֵ
	if (depth <= 0) return glm::vec3(0.3);
	//��
	Material material;
	glm::vec3 intersection = ray_scene_intersect(ray, models, material);
	//δ��������
	if (intersection == glm::vec3(0.0)) return glm::vec3(0.0);
	//���й�Դ
	if (material.is_luminous == true)
	{
		float length = glm::length(intersection - ray.origin);
		return material.albedo / (float)(length * length + 0.05);
	}

	//������
	Ray reflect_ray;
	reflect_ray.direction = random_vector(material.normal);
	//�����������direction�����ƶ�һ�㣬���⾫���������뷴�����ظ��ཻ
	reflect_ray.origin = intersection + (float)0.0001 * reflect_ray.direction;

	glm::vec3 color = rayTracing(reflect_ray, models, depth - 1);

	return color * material.albedo;
}//����
*/


glm::vec3 directLight(glm::vec3 V, glm::vec3 point, Material pM, const std::vector<model*>& models)
{
	glm::vec3 color(0.0);
	for (int i = 0; models[i]->material.is_luminous == true; i++)
	{
		triangle* T = (triangle*)models[i];

			glm::vec3 L = glm::normalize(T->point[0] - point);
			glm::vec3 H = glm::normalize(V + L);

			//����ֱ�����ӵ�͹�Դ������ּнǴ���90�ȵ����
			float NdotL = glm::dot(pM.normal, L);
			if (NdotL < 0.0) continue;

			//�ж��Ƿ��ڵ�
			Ray ray;
			ray.direction = L;
			ray.origin = point + (float)0.0001 * ray.direction;
			Material material;
			ray_scene_intersect(ray, models, material);
			if (material.is_luminous != true) continue;

			float length = glm::length(T->point[0] - point);
			glm::vec3 Li = T->material.albedo / (float)(length * length + 0.05);
			glm::vec3 Fr = BRDF(V, L, H, pM);
			color += Li * Fr * NdotL;
	}
	//���Ʒ���ֵ�ķ�Χ
	return color / (color + glm::vec3(1.0));
}

glm::vec3 __rendering(const Ray& ray, const std::vector<model*>& models, int depth)
{
	//��
	Material material;
	glm::vec3 intersection = ray_scene_intersect(ray, models, material);
	//δ��������
	if (intersection == glm::vec3(0.0)) return glm::vec3(0.0);
	//���й�Դ
	if (material.is_luminous == true)
	{
		float length = glm::length(intersection - ray.origin);
		//���ϳ���������ھ������ʱֵ����
		return material.albedo / (float)(length * length + 0.05);
	}

	//���ߵ��䣬�������ؿ��巽��������Ⱦ����;
	int num_sample = NUM_SAMPLE;

	glm::vec3 directColor(0.0);
	depth--;
	if (depth == 1)
	{
		//�����ڶ��ε������ֱ�ӹ��գ��������������ߵ���
		num_sample = 10;
		directColor = directLight((float)-1.0 * ray.direction, intersection, material, models);
	}
	//���һ�ε��䣬������ֱ�ӹ���
	if(depth == 0) return directLight((float)-1.0 * ray.direction, intersection, material, models);

	glm::vec3 color(0.0);
	float num = 0.0001;
	for (int i = 0; i < num_sample; i++)
	{
		float a = material.roughness * material.roughness;
		glm::vec3 H = ImportanceSampleGGX(Hammersley((uint32_t)i, num_sample), material.normal, a);
		glm::vec3 V = (float)-1.0 * ray.direction;
		glm::vec3 L = (float)2.0 * glm::dot(H, V) * H - V;

		float NdotH = glm::dot(material.normal, H);
		float NdotL = glm::dot(material.normal, L);
		float NdotV = glm::dot(material.normal, V);
		float HdotL = glm::dot(H, L);

		//�ų��޷��������׵����䷽��  �����ڶ�һ���������ɫʱ��Ӧ��ʱ��ע��L��V�ķ����Ƿ���ȷ
		if (NdotL < 0.0 || NdotV < 0.0) continue;

		Ray reflect_ray;
		reflect_ray.direction = L;
		//�����������direction�����ƶ�һ�㣬���⾫���������뷴�����ظ��ཻ
		reflect_ray.origin = intersection + (float)0.0001 * reflect_ray.direction;

		glm::vec3 Li = __rendering(reflect_ray, models, depth);
		glm::vec3 Fr = BRDF(V, L, H, material);
		float D = DistributionGGX(NdotH, a);
		float pdf = D *  NdotH / (4.0 * HdotL);

		num += 1;
		color += Li * Fr * NdotL / pdf;
	}
	//���������Ĳ�����
	color /= num;

	//����ֱ�ӹ������ӹ��յı�ֵ���Եõ��ȽϺ��ʵ�ֵ
	if (depth == 1)
	{
		color /= (color + glm::vec3(1.0));
		return T * directColor + (1 - T) * color;
	}

	return color;
}

glm::vec3 rendering(const Ray& ray, const std::vector<model*>& models)
{
	return __rendering(ray, models, DEPTH);
}
