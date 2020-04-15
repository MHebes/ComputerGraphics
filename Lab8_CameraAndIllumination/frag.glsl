#version 330
#define NR_POINT_LIGHTS 4

// Take in our texture coordinate from our vertex shader
in vec2 texCoords;
// And our normal
in vec3 norm;
// And our fragment position for lighting
in vec3 fragPos;

// We always define a fragment color that we output.
out vec4 fragColor;

// Define our light(s)
struct PointLight {
  vec3 position;
  float ambientIntensity;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

vec3 ComputeLighting(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);

  // diffuse shading
  float diff = max(dot(norm, lightDir), 0.0);

  // specular
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

  // attenuation
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + 
                      light.quadratic * (distance * distance));

  // combine
  vec3 ambient = light.ambient;
  vec3 diffuse = light.diffuse * diff;
  vec3 specular = light.specular * spec;

  return attenuation * (ambient + diffuse + specular);
}

// Maintain our uniforms.
uniform sampler2D tex;              // our primary texture
uniform mat4 view;                  // we need the view matrix for highlights
uniform PointLight pointLights[1];  // Our lights

void main() {
  // Set our output fragment color to whatever we pull from our input texture (Note, change 'tex' to whatever the sampler is named)
  vec3 diffuse = texture(tex, texCoords).rgb;

  // Calculate viewDir
  vec3 viewPos = vec3(inverse(view)[3]);
  vec3 viewDir = normalize(viewPos - fragPos);
  vec3 lighting = ComputeLighting(pointLights[0], norm, fragPos, viewDir);

  // final color + how dark or light
  fragColor = vec4(diffuse * lighting, 1.0);
}