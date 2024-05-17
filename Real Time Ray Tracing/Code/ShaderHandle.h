#pragma once
#include<glad/glad.h>
#include<string>
#include<sstream>
#include<fstream>
#include<iostream>
#include<vector>
#include <glm/gtc/type_ptr.hpp>

namespace ShaderHandle {

	//纹理数据管理
	struct Texture {
		std::string name;
		unsigned int id;
		unsigned int type;

		Texture(std::string _name, unsigned int _id, unsigned int _type) {
			name = _name;
			id = _id;
			type = _type;
		}
	};

	//生成着色器，以及着色器参数设置
	class Program {
	public:
		//着色器程序ID
		GLuint ID;
		Program(std::string vf, std::string ff, std::string gf = "NULL");
		~Program() { glDeleteProgram(ID); }
		//禁止复制
		Program(const Program&) = delete;
		Program& operator = (const Program&) = delete;

		void AddTexture(std::string name, unsigned int id, unsigned int type);

		void use(unsigned int VAO) {
			glBindVertexArray(VAO);
			glUseProgram(ID);
			SetTexture();
		}

		GLuint getID() { return ID; }
		void setMat(const char* str, glm::mat4 mat) {
			glUniformMatrix4fv(glGetUniformLocation(ID, str), 1, GL_FALSE, glm::value_ptr(mat));
		}
		void setVec3f(const char* str, glm::vec3 V) {
			glUniform3f(glGetUniformLocation(ID, str), V.x, V.y, V.z);
		}
		void setVec2f(const char* str, glm::vec2 V) {
			glUniform2f(glGetUniformLocation(ID, str), V.x, V.y);
		}
		void SetInt(const char* str, int value) {
			glUniform1i(glGetUniformLocation(ID, str), value);
		}
		void SetUInt(const char* str, unsigned int value) {
			glUniform1ui(glGetUniformLocation(ID, str), value);
		}

	private:
		std::vector<Texture> textures;
		unsigned int generateShader(std::string path, GLenum type);
		void SetTexture();
	};

	//加载顶点属性
	unsigned int LoadVertices(const std::vector<float>& data);
	//加载HDR贴图
	unsigned int LoadHDR(const char* filePath);
	//装入模型数据
	unsigned int LoadTextureBuffer(GLsizeiptr size, const void* data);
}


