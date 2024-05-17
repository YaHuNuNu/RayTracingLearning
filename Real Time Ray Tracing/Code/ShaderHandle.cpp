#include "ShaderHandle.h"
#include "hdrloader.h"

namespace ShaderHandle {
	Program::Program(std::string vf, std::string ff, std::string gf) {
		//������ɫ��
		unsigned int vShaderID = generateShader(vf, GL_VERTEX_SHADER);
		//Ƭ����ɫ��
		unsigned int fShaderID = generateShader(ff, GL_FRAGMENT_SHADER);
		//������ɫ��
		unsigned int gShaderID = 0;
		if (gf != "NULL")
			gShaderID = generateShader(gf, GL_GEOMETRY_SHADER);

		//������ɫ������
		ID = glCreateProgram();
		glAttachShader(ID, vShaderID);
		glAttachShader(ID, fShaderID);
		if (gShaderID)
			glAttachShader(ID, gShaderID);

		glLinkProgram(ID);
		//��ѯ�������Ӵ���
		int success;
		char infoLog[512];
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "Program link failed!\n" << infoLog << std::endl;
		}

		// ɾ����ɫ���������Ѿ����ӵ����ǵĳ����У�������Ҫ��
		glDeleteShader(vShaderID);
		glDeleteShader(fShaderID);
		if (gShaderID)
			glDeleteShader(gShaderID);
	}


	void Program::AddTexture(std::string name, unsigned int id, unsigned int type)
	{
		textures.push_back(Texture(name, id, type));
	}

	void Program::SetTexture()
	{
		for (int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(textures[i].type, textures[i].id);
			this->SetInt(textures[i].name.c_str(), i);
		}
	}

	unsigned int Program::generateShader(std::string path, GLenum type) {
		std::ifstream file;
		std::stringstream stream;

		//ȷ�������׳��쳣
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			file.open(path, std::ios::in);
			//���ı�һ���Զ���ķ���
			stream << file.rdbuf();
			file.close();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << path << std::endl << "Shader file read failed!\n";
		}

		std::string source = stream.str();
		const char* shaderSource = source.c_str();

		//��д��ɫ������
		unsigned int shaderID;
		GLint success;
		GLchar infoLog[512];

		shaderID = glCreateShader(type);
		glShaderSource(shaderID, 1, &shaderSource, NULL);
		glCompileShader(shaderID);

		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
			std::cout << path << std::endl << infoLog << std::endl;
		}

		return shaderID;
	}

	//���ڴ��������
	unsigned int LoadVertices(const std::vector<float> &data)
	{
		unsigned int VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);//��Ҫ
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);

		return VAO;
	}

	unsigned int LoadHDR(const char* filePath)
	{
		HDRLoaderResult hdrData;
		if (!HDRLoader::load(filePath, hdrData))
			std::cout << filePath << std::endl << "HDR file read failed!\n";

		unsigned int HDR_TEX;
		glGenTextures(1, &HDR_TEX);
		glBindTexture(GL_TEXTURE_2D, HDR_TEX);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, hdrData.width, hdrData.height, 0, GL_RGB, GL_FLOAT, hdrData.cols);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		return HDR_TEX;
	}


	unsigned int LoadTextureBuffer(GLsizeiptr size, const void *data)
	{
		unsigned int TBO;
		glGenBuffers(1, &TBO);
		glBindBuffer(GL_TEXTURE_BUFFER, TBO);
		glBufferData(GL_TEXTURE_BUFFER, size, data, GL_STATIC_DRAW);
		unsigned int TBO_TEX;
		glGenTextures(1, &TBO_TEX);
		glBindTexture(GL_TEXTURE_BUFFER, TBO_TEX);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, TBO);

		return TBO_TEX;
	}
}