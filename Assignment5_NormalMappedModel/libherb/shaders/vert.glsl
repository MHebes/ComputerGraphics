#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;
layout(location = 3) in vec3 tanget;
layout(location = 4) in vec3 bitangent;

// We now have our camera system set up.
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// We define a new output vec2 for our texture coordinates.
out vec2 texCoords;
out vec3 fragPos;
out mat3 TBN; // for normals

void main()
{
    // We have our transformed position set properly now
    gl_Position = projectionMatrix*viewMatrix*modelMatrix*vec4(position, 1.0);
    
    // Our fragment pos for lighting.
    fragPos = (modelMatrix*vec4(position, 1.0)).xyz;

    // calculate tbn matrix
    vec3 T = normalize(vec3(modelMatrix * vec4(tanget,    0.0)));
    vec3 B = normalize(vec3(modelMatrix * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(modelMatrix * vec4(normal,    0.0)));

    TBN = mat3(T, B, N);

    // And we map our texture coordinates as appropriate
    texCoords = textureCoords;
}