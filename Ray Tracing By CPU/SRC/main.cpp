#include<glm/glm.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>
#include<vector>
#include <thread>
#include "scene.h"
#include "rendering.h"

const int WIDTH = 512;
const int HEIGHT = 512;
const int Channel = 3;
unsigned char* data = new unsigned char[WIDTH * HEIGHT * Channel];

//相机和投影屏位置
const float Proj_Surface = 1.1;
const glm::vec3 EYE = glm::vec3(0, 0, 4.5);
//gama校正参数
const float gama = 2.2;
const glm::vec3 inverse_gama(1.0 / gama);

void process(int start, int end);

int main()
{
    loadScene();

    const int num_elements = HEIGHT; // 待处理的元素总数
    const int num_threads = 20;   // 使用的线程数
    std::vector<std::thread> threads;
    // 计算每个线程应处理的元素数量
    int num_elements_per_thread = num_elements / num_threads;
    int remainder = num_elements % num_threads;

    for (int i = 0; i < num_threads; i++) {
        int start = i * num_elements_per_thread;
        int end = (i + 1) * num_elements_per_thread;
        // 如果有余数，将其分配给最后一个线程
        if (i == num_threads - 1 && remainder != 0) {
            end += remainder;
        }
        // 创建并启动线程
        threads.push_back(std::thread(process, start, end));
    }

     printf("加载中······\n");
    // 等待所有线程完成
    for (auto& thread : threads)
        thread.join();

    stbi_write_png("result.png", WIDTH, HEIGHT, Channel, data, WIDTH * Channel);
    delete[]data;
    clearScene();
    printf("加载完成！\n");
    return 0;
}

void process(int start, int end){
    for (int j = start; j < end; j++)
    {
        for (int i = 0; i < WIDTH; i++)
        {
            //将坐标变换到[-1,1]内
            float x = (((float)i + 0.5) / (float)WIDTH) * 2.0 - 1.0;
            float y = (((float)j + 0.5) / (float)HEIGHT) * 2.0 - 1.0;

            Ray ray;
            ray.origin = glm::vec3(x, y, Proj_Surface);
            ray.direction = glm::normalize(ray.origin - EYE);

            glm::vec3 color = rendering(ray, models);

            // Reinhard色调映射
            color = color / (color + glm::vec3(1.0));
            //gama校正
            color = glm::pow(color, inverse_gama);

            int index = ((HEIGHT - 1 - j) * WIDTH + i) * Channel;
            data[index++] = (unsigned char)(color.x * 255);
            data[index++] = (unsigned char)(color.y * 255);
            data[index++] = (unsigned char)(color.z * 255);
        }
    }
}