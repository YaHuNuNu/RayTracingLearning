#pragma once

#include<glm/glm.hpp>
#include<vector>
#include "model.h"


//均匀随机生成给定范围的值
float random_float(float left = 0.0, float right = 1.0);

//在给定法向量的单位半球内均匀采样
glm::vec3 random_vector(glm::vec3 normal);

//使用Hammersley函数生成二维的低差异序列，用于GGX重要性采样  PS：由于算法使用位运算，所以应确保形参为32位的unsigned int
glm::vec2 Hammersley(uint32_t i, int num_sample);

//针对GGX函数的重要性采样，返回值是半程向量h
glm::vec3 ImportanceSampleGGX(glm::vec2 Xi, glm::vec3 N, float a);

//菲涅尔项F，采用Schlick近似
glm::vec3 fresnelSchlick(float HdotV, glm::vec3 F0);

//法线分布函数GGX
float DistributionGGX(float NdotH, float a);

//几何遮蔽G，采用与GGX函数相匹配的Schlick-GGX
float GeometrySchlickGGX(float NdotV, float k);

//考虑观察方向和光线方向结合
float GeometrySmith(float NdotV, float NdotL, float a);

//Cook-Torrance BRDF
glm::vec3 BRDF(glm::vec3 V, glm::vec3 L, glm::vec3 H, const Material& material);

//光线与场景中的多个模型求交
glm::vec3 ray_scene_intersect(const Ray& ray, const std::vector<model*>& models, Material& material);

//单光线路径追踪
//glm::vec3 rayTracing(const Ray& ray, const std::vector<model*>& models, int depth);

//使用渲染方程的直接光照计算，光源只考虑三角形
glm::vec3 directLight(glm::vec3 V, glm::vec3 point, Material pM, const std::vector<model*>& lightModels);

//对从摄像头中射出的射线进行渲染。
glm::vec3 __rendering(const Ray& ray, const std::vector<model*>& models, int depth);

//供调用
glm::vec3 rendering(const Ray& ray, const std::vector<model*>& models);