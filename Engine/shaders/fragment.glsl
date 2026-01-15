#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;

void main(){
    vec3 norm = normalize(Normal);
    vec3 lightdir = normalize(lightPos-FragPos);

    float diff = max(dot(norm, lightdir), 0.0);
    vec3 diffuse = diff*vec3(1.0,1.0,1.0);

    float ambientstrength = 0.1;
    vec3 ambient = ambientstrength*vec3(1.0, 1.0, 1.0);

    vec3 objectColor = vec3(1.0, 0.5, 0.31);
    vec3 result = (ambient+diffuse)*objectColor;
    FragColor = vec4(result, 1.0f);
}