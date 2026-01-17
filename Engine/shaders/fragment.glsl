#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 viewPos; 
uniform sampler2D texture1;

void main()
{
    // ---------------------------------------------------------
    // 1. SETUP DATA
    // ---------------------------------------------------------
    // If you didn't fix the tiling in the Vertex Shader, uncomment the line below:
    // vec2 UV = TexCoord * 0.1; 
    vec2 UV = TexCoord; // Use this if you fixed it in Vertex Shader

    vec3 color = texture(texture1, UV).rgb;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    // ---------------------------------------------------------
    // 2. AMBIENT LIGHTING
    // ---------------------------------------------------------
    // Base light so shadows aren't pitch black
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);

    // ---------------------------------------------------------
    // 3. DIFFUSE LIGHTING (The Sun)
    // ---------------------------------------------------------
    // "max" ensures the light doesn't become negative (which would darken the ambient)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

    // ---------------------------------------------------------
    // 4. SPECULAR LIGHTING (The Shine)
    // ---------------------------------------------------------
    float specularStrength = 0.5;
    
    // Blinn-Phong reflection vector
    vec3 reflectDir = reflect(-lightDir, norm);  
    
    // 32.0 = Shininess. Higher = smaller, sharper highlight. Lower = spread out.
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);  

    // ---------------------------------------------------------
    // 5. COMBINE RESULTS
    // ---------------------------------------------------------
    // Multiply the texture color by the light (Ambient + Diffuse)
    // Add the Specular (shine) on top so it looks like white light reflecting off the surface
    vec3 result = (ambient + diffuse) * color + specular;
    
    FragColor = vec4(result, 1.0);
}