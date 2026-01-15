#define FAST_OBJ_IMPLEMENTATION
#include <fast_obj/fast_obj.h>
#include <glad/glad.h>
#include <stdio.h>
#include "model.h"

Model load_model(const char* filepath){
    Model m = {0};

    fastObjMesh* mesh = fast_obj_read(filepath);
    if(!mesh){
        printf("Error model load failed!\n");
        return m;
    }

    GLuint totalVertices = 0;
    for(unsigned int i = 0; i<mesh->face_count; ++i){
        GLuint fv = mesh->face_vertices[i];
        if(fv==3)totalVertices+=3;
        else if(fv==4)totalVertices+=6;
    }

    m.vertexCount = totalVertices;

    float* data = (float*)malloc(totalVertices*6*sizeof(float));

    GLuint dataIndex = 0;
    GLuint indexOffset = 0;

    for(unsigned int i = 0; i<mesh->face_count; ++i){
        GLuint fv = mesh->face_vertices[i];

        int triangleIndices[][3] = {{0, 1, 2}, {0, 2, 3}};
        int trianglesToDraw = (fv == 4) ? 2 : 1;

        for (int t = 0; t < trianglesToDraw; ++t) {
            for (int j = 0; j < 3; ++j) {
                fastObjIndex idx = mesh->indices[indexOffset + triangleIndices[t][j]];

                // POSITION
                data[dataIndex++] = mesh->positions[3 * idx.p + 0];
                data[dataIndex++] = mesh->positions[3 * idx.p + 1];
                data[dataIndex++] = mesh->positions[3 * idx.p + 2];

                // NORMAL (Check if exists, otherwise default to Up)
                if (mesh->normal_count > 0) {
                    data[dataIndex++] = mesh->normals[3 * idx.n + 0];
                    data[dataIndex++] = mesh->normals[3 * idx.n + 1];
                    data[dataIndex++] = mesh->normals[3 * idx.n + 2];
                } else {
                    data[dataIndex++] = 0.0f; 
                    data[dataIndex++] = 1.0f; 
                    data[dataIndex++] = 0.0f;
                }
            }
        }

        indexOffset += fv;
    }

    glGenVertexArrays(1, &m.VAO);
    glGenBuffers(1, &m.VBO);

    glBindVertexArray(m.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
    glBufferData(GL_ARRAY_BUFFER, totalVertices*6*sizeof(float), data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    fast_obj_destroy(mesh);
    free(data);

    printf("Loaded Model: %s (%d vertices)\n", filepath, totalVertices);
    return m;
}