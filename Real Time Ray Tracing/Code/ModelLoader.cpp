#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "ModelLoader.h"
#include <algorithm>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace ModelLoad {
    //AABB树中叶子节点包含的最大三角形个数
    const int NUM = 6;

    bool LoadByTinyObj(const char* filePath, ModelLoaderResult& res, glm::mat4 trans/* = glm::mat4(1.0f)*/)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath);

        if (!err.empty()) {
            printf("err: %s\n", err.c_str());
        }

        if (!ret) {
            printf("failed to load : %s\n", filePath);
            return false;
        }

        if (shapes.size() == 0) {
            printf("err: # of shapes are zero.\n");
            return false;
        }

        //将顶点数据按三角形组合读取到vertices中
        std::vector<glm::vec3> vertices;

        for (const tinyobj::shape_t& shape : shapes) {
            const std::vector<tinyobj::index_t>& indices = shape.mesh.indices;
            for (int i = 0; i < indices.size(); i++) {
                glm::vec3 vertex;
                int index = indices[i].vertex_index;
                vertex.x = attrib.vertices[index * 3 + 0];
                vertex.y = attrib.vertices[index * 3 + 1];
                vertex.z = attrib.vertices[index * 3 + 2];
                glm::vec4 vertex_T = trans * glm::vec4(vertex, 1);
                vertex.x = vertex_T.x;
                vertex.y = vertex_T.y;
                vertex.z = vertex_T.z;
                vertices.push_back(vertex);
            }
        }

        //组成三角形
        for (int i = 0; i < vertices.size() / 3; i++) {
            Triangle triangle;
            triangle.vertex[0] = vertices[i * 3 + 0];
            triangle.vertex[1] = vertices[i * 3 + 1];
            triangle.vertex[2] = vertices[i * 3 + 2];
            //构建法向量
            triangle.normal[0] = glm::normalize(glm::cross(triangle.vertex[1] - triangle.vertex[0], triangle.vertex[2] - triangle.vertex[0]));
            triangle.normal[1] = triangle.normal[0];
            triangle.normal[2] = triangle.normal[0];

            res.triangles.push_back(triangle);
        }

        return true;
    }

    void ProcessNode(aiNode* node, const aiScene* scene, ModelLoaderResult& res);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene, ModelLoaderResult& res);

    bool LoadByAssimp(const char* filePath, ModelLoaderResult& res) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return false;
        }

        ProcessNode(scene->mRootNode, scene, res);
        return true;
    }

    void ProcessNode(aiNode* node, const aiScene* scene, ModelLoaderResult& res) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(mesh, scene, res);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ProcessNode(node->mChildren[i], scene, res);
    }

    void ProcessMesh(aiMesh* mesh, const aiScene* scene, ModelLoaderResult& res) {
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j += 3) {
                Triangle triangle;
                for (int t = 0; t < 3; t++) {
                    unsigned int index = face.mIndices[j + t];
                    triangle.vertex[t].x = mesh->mVertices[index].x;
                    triangle.vertex[t].y = mesh->mVertices[index].y;
                    triangle.vertex[t].z = mesh->mVertices[index].z;

                    if (mesh->HasNormals()) {
                        triangle.normal[t].x = mesh->mNormals[index].x;
                        triangle.normal[t].y = mesh->mNormals[index].y;
                        triangle.normal[t].z = mesh->mNormals[index].z;
                    }
                }

                res.triangles.push_back(triangle);
            }
        }

        //HasTextureCoords(i) 第i组纹理坐标是否存在，在这里我们只用第一组
            /*if (mesh->HasTextureCoords(0)) {
                vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
                vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoord.z = 0;
            }
            else {
                vertex.texCoord = glm::vec3(0.0f, 0.0f, 0.0f);
            }

            if (mesh->HasTangentsAndBitangents()) {
                vertex.tangent.x = mesh->mTangents[i].x;
                vertex.tangent.y = mesh->mTangents[i].y;
                vertex.tangent.z = mesh->mTangents[i].z;

                vertex.bitangent.x = mesh->mBitangents[i].x;
                vertex.bitangent.y = mesh->mBitangents[i].y;
                vertex.bitangent.z = mesh->mBitangents[i].z;
            }
            else {
                vertex.tangent = glm::vec3(0.0f);
                vertex.bitangent = glm::vec3(0.0f);
            }*/
    }

    bool CompareX(const Triangle& t1, const Triangle& t2) {
        return t1.GetCenter().x < t2.GetCenter().x;
    }

    bool CompareY(const Triangle& t1, const Triangle& t2) {
        return t1.GetCenter().y < t2.GetCenter().y;
    }

    bool CompareZ(const Triangle& t1, const Triangle& t2) {
        return t1.GetCenter().z < t2.GetCenter().z;
    }

    int __Build(ModelLoaderResult& res, int l, int r) {
        AABBNode node;
        node.triangleIndex.x = float(l);
        node.triangleIndex.y = float(r);
        glm::vec3 min = res.triangles[l].GetMin(), max = res.triangles[l].GetMax();
        for (int i = l + 1; i <= r; i++) {
            min = glm::min(min, res.triangles[i].GetMin());
            max = glm::max(max, res.triangles[i].GetMax());
        }
        node.min = min;
        node.max = max;
        res.nodeData.push_back(node);
        int index = res.nodeData.size() - 1;

        if (r - l + 1 <= NUM)
            return index;

        glm::vec3 delta = max - min;
        int maxAxle = 0;
        bool(*Compare)(const Triangle&, const Triangle&);
        if (delta.x > delta.y && delta.x > delta.z) {
            Compare = CompareX;
            maxAxle = 0;
        }
        else if (delta.y > delta.x && delta.y > delta.z) {
            Compare = CompareY;
            maxAxle = 1;
        }
        else {
            Compare = CompareZ;
            maxAxle = 2;
        }
        std::sort(&res.triangles[l], &res.triangles[r] + 1, Compare);

        //依照选定轴的差值的一半将三角形分为两部分
        float midValue = (res.triangles[l].GetCenter()[maxAxle] + res.triangles[r].GetCenter()[maxAxle]) / 2.0f;
        int mid = r;
        for (int i = l; i <= r; i++) {
            //此时normal[0]中暂存的是center
            if (res.triangles[i].GetCenter()[maxAxle] > midValue) {
                mid = i;
                break;
            }
        }

        //考虑mid = r的极端情况，左子树只能为mid - 1;
        int child_l = __Build(res, l, mid - 1);
        int child_r = __Build(res, mid, r);
        res.nodeData[index].child.x = float(child_l);
        res.nodeData[index].child.y = float(child_r);

        return index;
    }


    bool BuildAABB(ModelLoaderResult& res)
    {
        if (res.triangles.empty()) {
            printf("ModelLoaderResult is empty!\n");
            return false;
        }

        __Build(res, 0, res.triangles.size() - 1);

        return true;
    }

}