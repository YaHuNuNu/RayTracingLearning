#pragma once

#include<glm/glm.hpp>
#include<vector>
#include "model.h"


//����������ɸ�����Χ��ֵ
float random_float(float left = 0.0, float right = 1.0);

//�ڸ����������ĵ�λ�����ھ��Ȳ���
glm::vec3 random_vector(glm::vec3 normal);

//ʹ��Hammersley�������ɶ�ά�ĵͲ������У�����GGX��Ҫ�Բ���  PS�������㷨ʹ��λ���㣬����Ӧȷ���β�Ϊ32λ��unsigned int
glm::vec2 Hammersley(uint32_t i, int num_sample);

//���GGX��������Ҫ�Բ���������ֵ�ǰ������h
glm::vec3 ImportanceSampleGGX(glm::vec2 Xi, glm::vec3 N, float a);

//��������F������Schlick����
glm::vec3 fresnelSchlick(float HdotV, glm::vec3 F0);

//���߷ֲ�����GGX
float DistributionGGX(float NdotH, float a);

//�����ڱ�G��������GGX������ƥ���Schlick-GGX
float GeometrySchlickGGX(float NdotV, float k);

//���ǹ۲췽��͹��߷�����
float GeometrySmith(float NdotV, float NdotL, float a);

//Cook-Torrance BRDF
glm::vec3 BRDF(glm::vec3 V, glm::vec3 L, glm::vec3 H, const Material& material);

//�����볡���еĶ��ģ����
glm::vec3 ray_scene_intersect(const Ray& ray, const std::vector<model*>& models, Material& material);

//������·��׷��
//glm::vec3 rayTracing(const Ray& ray, const std::vector<model*>& models, int depth);

//ʹ����Ⱦ���̵�ֱ�ӹ��ռ��㣬��Դֻ����������
glm::vec3 directLight(glm::vec3 V, glm::vec3 point, Material pM, const std::vector<model*>& lightModels);

//�Դ�����ͷ����������߽�����Ⱦ��
glm::vec3 __rendering(const Ray& ray, const std::vector<model*>& models, int depth);

//������
glm::vec3 rendering(const Ray& ray, const std::vector<model*>& models);