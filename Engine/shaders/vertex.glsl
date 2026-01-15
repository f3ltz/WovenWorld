#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;      // Replaces 'transform'
uniform mat4 view;       // The Camera
uniform mat4 projection; // The Lens

void main()
{
    // Multiplication order matters! P * V * M * Vector
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}