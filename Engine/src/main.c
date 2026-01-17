#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>
#include "model.h"

// Camera state
vec3 cameraPos   = {0.291234f, 22.452366f, 24.892710f};
vec3 cameraFront = {0.0f, 0.0f, -1.0f};
vec3 cameraUp    = {0.0f, 1.0f,  0.0f};
float deltaTime  = 0.0f;
float lastFrame  = 0.0f;

// Mouse state
float lastX = 400.0f, lastY = 300.0f; // Center of screen
float yaw   = -90.0f; // -90 points to negative Z (forward)
float pitch =  0.0f;
int firstMouse = 1; // bool to check if it's the very first frame

char* readShaderSource(const char* filePath){
    FILE* file = fopen(filePath, "rb");
    if(!file){
        printf("Error, file not found: %s\n", filePath);
        return NULL;
    }
    //count bytes
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(length+1);
    if(!buffer){
        printf("Memory allocation failed!\n");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);
    return buffer;
}

GLuint compile_shader(const char* source, GLenum type){
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success){
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        fprintf(stderr, "SHADER_LOADER: Shader compilation failed:%s\n", info_log);
        return 0;
    }
    return shader;
}

GLuint shader_create_program(const char* vertex_path, const char* fragment_path){
    // Read shader files from disk
    char* vertex_source = readShaderSource(vertex_path);
    char* fragment_source = readShaderSource(fragment_path);
    if(!vertex_source || !fragment_source){
        free(vertex_source);
        free(fragment_source);
        return 0;
    }

    // Compile shaders
    GLuint vertex_shader = compile_shader(vertex_source, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(fragment_source, GL_FRAGMENT_SHADER);
    free(vertex_source);
    free(fragment_source);
    if(vertex_shader==0 || fragment_shader==0){
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }

    // Link shaders to a program
    GLuint program = glCreateProgram();
    glAttachShader(program,vertex_shader);
    glAttachShader(program,fragment_shader);
    glLinkProgram(program);

    // Check for success
    GLuint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success){
        char info_log[512];
        glGetProgramInfoLog(program, 512, NULL, info_log);
        fprintf(stderr, "SHADER_LOADER: Shader program linking failed:\n%s\n", info_log);
    }

    // Delete individual shaders as they are now linked to the program
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    if(!success){
        glDeleteProgram(program);
        return 0;
    }

    printf("SHADER_LOADER: Successfully created shader program.\n");
    return program;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = 0;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sens = 0.1f;
    xoffset*= sens;
    yoffset*= sens;

    yaw += xoffset;
    pitch += yoffset;

    // Constraint Pitch (prevent screen flipping)
    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    // Calculate the new Front vector (Trigonometry ahead!)
    vec3 direction;
    direction[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch)); // x
    direction[1] = sin(glm_rad(pitch));                     // y
    direction[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch)); // z
    
    glm_vec3_normalize_to(direction, cameraFront);

}

GLuint load_texture(const char* path){
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(1);

    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if(data){
        GLenum format = (nrChannels==4)?GL_RGBA:GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        printf("Texture failed to load at path: %s\n", path);
        stbi_image_free(data);
    }

    return textureID;
}

int main(){
    if(!glfwInit()){
        printf("Failed to init GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Engine", NULL, NULL);
    if(!window){
        printf("Failed to create window\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        printf("Failed to initialize Glad\n");
        return -1;
    }
    // Captures the mouse and hides the cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glEnable(GL_DEPTH_TEST);

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    GLuint shaderProgram = shader_create_program(
        "../shaders/vertex.glsl",
        "../shaders/fragment.glsl"
    );
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc  = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc  = glGetUniformLocation(shaderProgram, "projection");
    GLuint lightLoc = glGetUniformLocation(shaderProgram, "lightPos");

    Model myModel = load_model("../assets/peng.obj");  
    GLuint diffuseMap = load_texture("../assets/peng.png");

    // GLuint VBO, VAO;
    // glGenVertexArrays(1, &VAO);
    // glGenBuffers(1, &VBO);

    // glBindVertexArray(VAO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

    vec3 lightPos = {2.0f, 2.0f, 2.0f};

    while(!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        char title[64];
        sprintf(title, "FPS: %.2f", 1.0/deltaTime);
        glfwSetWindowTitle(window, title);


        float cameraSpeed = 25*deltaTime;
        if(glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS){
            glm_vec3_muladds(cameraFront, cameraSpeed, cameraPos);
        }
        if(glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS){
            glm_vec3_muladds(cameraFront, -cameraSpeed, cameraPos);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            vec3 crossProduct;
            glm_vec3_cross(cameraFront, cameraUp, crossProduct);
            glm_vec3_normalize(crossProduct);
            glm_vec3_muladds(crossProduct, -cameraSpeed, cameraPos);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            vec3 crossProduct;
            glm_vec3_cross(cameraFront, cameraUp, crossProduct);
            glm_vec3_normalize(crossProduct);
            glm_vec3_muladds(crossProduct, cameraSpeed, cameraPos);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){       
            glm_vec3_muladds(cameraUp, cameraSpeed, cameraPos);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
            glm_vec3_muladds(cameraUp, -cameraSpeed, cameraPos);
        }

        

        mat4 projection;
        glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float*)projection);
        // 1. FIX: Send the actual Light Position (not 0,0,0)
        glUniform3fv(lightLoc, 1, (float*)lightPos); 

        // 2. FIX: Send Camera Position for Specular Lighting
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, (float*)cameraPos);
        
        mat4 view;
        vec3 center;
        glm_vec3_add(cameraPos, cameraFront, center);
        glm_lookat(cameraPos, center, cameraUp, view);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);

        mat4 model;
        glm_mat4_identity(model);
        //glm_rotate(model, (float)glfwGetTime(), (vec3){0.5f, 1.0f, 0.0f});
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glBindVertexArray(myModel.VAO);
        glDrawArrays(GL_TRIANGLES, 0, myModel.vertexCount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}