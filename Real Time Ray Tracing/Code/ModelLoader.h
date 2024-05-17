#pragma once
#include<vector>
#include<glm/glm.hpp>

namespace ModelLoad {

	struct Triangle {
		glm::vec3 vertex[3];
		glm::vec3 normal[3];

		//用于计算包围盒
		glm::vec3 GetMin() const {
			return glm::min(vertex[2], glm::min(vertex[0], vertex[1]));
		}
		glm::vec3 GetMax() const {
			return glm::max(vertex[2], glm::max(vertex[0], vertex[1]));
		}

		glm::vec3 GetCenter() const {
			return (vertex[0] + vertex[1] + vertex[2]) / 3.0f;
		}
	};

	struct AABBNode {
		glm::vec3 min, max;
		glm::vec3 child; // X为左子节点，Y为右子节点，Z为空
		glm::vec3 triangleIndex; // X为左边界索引，Y为右边界索引，Z为空

		AABBNode() {
			child = glm::vec3(-1, -1, -1);
			min = glm::vec3(0, 0, 0);
			max = glm::vec3(0, 0, 0);
			triangleIndex = glm::vec3(0, 0, 0);
		}
	};

	class ModelLoaderResult {
	public:
		std::vector<Triangle> triangles;
		std::vector<AABBNode> nodeData;
	};

	bool LoadByTinyObj(const char* filePath, ModelLoaderResult& res, glm::mat4 trans = glm::mat4(1.0f));
	bool LoadByAssimp(const char* filePath, ModelLoaderResult& res);
	bool BuildAABB(ModelLoaderResult& res);
}
