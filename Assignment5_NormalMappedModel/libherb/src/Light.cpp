#include "Light.h"

#include <iostream>
#include <QOpenGLShaderProgram>

Light::Light()
    : position(0, 0, 0),
      ambientIntensity(0.25),
      diffuseIntensity(1),
      specularIntensity(1),
      constant(1.0f),
      linear(0.09f),
      quadratic(0.032f),
      ambient(1, 1, 1),
      diffuse(1, 1, 1),
      specular(1, 1, 1)
{
}

void Light::applyToUniform(QOpenGLShaderProgram& shader,
                           std::string uniform) const

{
  shader.setUniformValue((uniform + ".position").c_str(), position);
  shader.setUniformValue((uniform + ".ambientIntensity").c_str(),
                         ambientIntensity);
  shader.setUniformValue((uniform + ".diffuseIntensity").c_str(),
                         diffuseIntensity);
  shader.setUniformValue((uniform + ".specularIntensity").c_str(),
                         specularIntensity);
  shader.setUniformValue((uniform + ".constant").c_str(), constant);
  shader.setUniformValue((uniform + ".linear").c_str(), linear);
  shader.setUniformValue((uniform + ".quadratic").c_str(), quadratic);
  shader.setUniformValue((uniform + ".ambient").c_str(), ambient);
  shader.setUniformValue((uniform + ".diffuse").c_str(), diffuse);
  shader.setUniformValue((uniform + ".specular").c_str(), specular);
}

void Light::setRange(float range) {
  constant = 1.0f;
  linear = 5.0f / range;
  quadratic = 2.0f / range;
}