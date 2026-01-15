#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;      // Replaces 'transform'
uniform mat4 view;       // The Camera
uniform mat4 projection; // The Lens

out vec3 Normal;
out vec3 FragPos;

void main()
{   
    FragPos = vec3(model*vec4(aPos, 1.0));

    // This handles non-uniform scaling correctly
    Normal = mat3(transpose(inverse(model))) * aNormal;
    // Multiplication order matters! P * V * M * Vector
    gl_Position = projection * view *  vec4(aPos, 1.0);
}