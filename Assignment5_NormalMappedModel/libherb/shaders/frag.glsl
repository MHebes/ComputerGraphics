#version 330
#define MAX_LIGHTS 10

// Credit to https://learnopengl.com/Lighting/Basic-Lighting for much of the lighting calcs

// Take in our texture coordinate from our vertex shader
in vec2 texCoords;
// And our normal
in vec3 norm;
// And our fragment position for lighting
in vec3 fragPos;

// We always define a fragment color that we output.
out vec4 fragColor;

// Define our light(s)
struct Light {
  vec3 position;
  float ambientIntensity;
  float diffuseIntensity;
  float specularIntensity;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

vec3 ComputeLighting(Light light, vec3 norm, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);

  // diffuse
  float diff = max(dot(norm, lightDir), 0.0);

  // specular
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

  // combine
  vec3 ambient = light.ambient * light.ambientIntensity;
  vec3 diffuse = light.diffuse * diff * light.diffuseIntensity;
  vec3 specular = light.specular * spec * light.specularIntensity;

  // attenuation
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + 
                      light.quadratic * (distance * distance));

  return attenuation*(ambient + diffuse + specular);
}

// Maintain our uniforms.
uniform sampler2D tex;              // our primary texture
// uniform sampler2D normalMap;        // normal map

uniform vec3 viewPos;               // can't calculate from view
uniform Light lights[MAX_LIGHTS];  // Our lights
uniform int numLights; // number of lights

void main() {
  // Set our output fragment color to whatever we pull from our input texture (Note, change 'tex' to whatever the sampler is named)
  vec3 diffuse = texture(tex, texCoords).rgb;
  // vec3 normal = texture(normalMap, texCoords).rgb;
  // move to range [-1,1] from [0,1]
  // normal = normalize(normal * 2.0 - 1.0);

  // Calculate viewDir
  vec3 viewDir = normalize(viewPos - fragPos);

  vec3 lighting = vec3(0.0, 0.0, 0.0);

  for (int i = 0; i < numLights; i++)
    lighting += ComputeLighting(lights[i], norm, fragPos, viewDir);

  // final color + how dark or light
  fragColor = vec4(diffuse * lighting, 1.0);
}