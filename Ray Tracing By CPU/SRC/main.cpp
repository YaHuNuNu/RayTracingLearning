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

//�����ͶӰ��λ��
const float Proj_Surface = 1.1;
const glm::vec3 EYE = glm::vec3(0, 0, 4.5);
//gamaУ������
const float gama = 2.2;
const glm::vec3 inverse_gama(1.0 / gama);

void process(int start, int end);

int main()
{
    loadScene();

    const int num_elements = HEIGHT; // �������Ԫ������
    const int num_threads = 20;   // ʹ�õ��߳���
    std::vector<std::thread> threads;
    // ����ÿ���߳�Ӧ�����Ԫ������
    int num_elements_per_thread = num_elements / num_threads;
    int remainder = num_elements % num_threads;

    for (int i = 0; i < num_threads; i++) {
        int start = i * num_elements_per_thread;
        int end = (i + 1) * num_elements_per_thread;
        // ����������������������һ���߳�
        if (i == num_threads - 1 && remainder != 0) {
            end += remainder;
        }
        // �����������߳�
        threads.push_back(std::thread(process, start, end));
    }

     printf("�����С�����������\n");
    // �ȴ������߳����
    for (auto& thread : threads)
        thread.join();

    stbi_write_png("result.png", WIDTH, HEIGHT, Channel, data, WIDTH * Channel);
    delete[]data;
    clearScene();
    printf("������ɣ�\n");
    return 0;
}

void process(int start, int end){
    for (int j = start; j < end; j++)
    {
        for (int i = 0; i < WIDTH; i++)
        {
            //������任��[-1,1]��
            float x = (((float)i + 0.5) / (float)WIDTH) * 2.0 - 1.0;
            float y = (((float)j + 0.5) / (float)HEIGHT) * 2.0 - 1.0;

            Ray ray;
            ray.origin = glm::vec3(x, y, Proj_Surface);
            ray.direction = glm::normalize(ray.origin - EYE);

            glm::vec3 color = rendering(ray, models);

            // Reinhardɫ��ӳ��
            color = color / (color + glm::vec3(1.0));
            //gamaУ��
            color = glm::pow(color, inverse_gama);

            int index = ((HEIGHT - 1 - j) * WIDTH + i) * Channel;
            data[index++] = (unsigned char)(color.x * 255);
            data[index++] = (unsigned char)(color.y * 255);
            data[index++] = (unsigned char)(color.z * 255);
        }
    }
}