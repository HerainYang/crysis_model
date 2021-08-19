//
// Created by herain on 7/12/21.
//

#ifndef OPENGL_MESH_H
#define OPENGL_MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "Shader.h"

using namespace glm;
using namespace std;

struct Vertex{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoords;
};

struct Texture{
    unsigned int id;
    //这个id指的是以前我们用SOIL读取图片之后的返回值，即索引texture的值
    string type;
    //这个type会指明这个材质是漫反射还是镜面反射，之后这个会被用于命名
    string path;
};

class Mesh{
public:
    //网格数据
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    //函数
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    void Draw(Shader shader);

private:
    //渲染数据
    unsigned int VAO, VBO, EBO;
    //函数
    void setupMesh();
};

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures){
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::Draw(Shader shader) {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < textures.size(); i++){
        glActiveTexture(GL_TEXTURE0 + i);
        string number;
        string name = textures[i].type;
        if(name == "texture_diffuse")
            number = to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = to_string(specularNr++);

        shader.setFloat(("material."+name+number).c_str(), i);
        //每个漫反射纹理会被命名为texture_diffuseN，每个镜面反射纹理会被命名为texture_specularN
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        //将纹理数据传入到缓冲中，此时缓冲绑定着某个纹理
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

#endif //OPENGL_MESH_H


