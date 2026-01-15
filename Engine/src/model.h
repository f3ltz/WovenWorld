typedef struct 
{
    unsigned int VAO;
    unsigned int VBO;
    unsigned int vertexCount;
} Model;

Model load_model(const char* filepath);
void model_draw(Model* m, unsigned int shaderProgram);
void model_free(Model* m);
