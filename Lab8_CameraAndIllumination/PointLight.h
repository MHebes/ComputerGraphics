#pragma once

#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <string>

struct PointLight {
  QVector3D position;
  float ambientIntensity;
  float constant;
  float linear;
  float quadratic;
  QVector3D ambient;
  QVector3D diffuse;
  QVector3D specular;
};

// e.g. pointlight_set_uniform(shader, light, "pointLights[0]")
inline void pointlight_set_uniform(QOpenGLShaderProgram& shader, const PointLight& light, std::string uniformName) {
  shader.setUniformValue((uniformName + ".position").c_str(),   light.position);
  shader.setUniformValue((uniformName + ".ambientIntensity").c_str(),   light.ambientIntensity);
  shader.setUniformValue((uniformName + ".constant").c_str(),   light.constant);
  shader.setUniformValue((uniformName + ".linear").c_str(), light.linear);
  shader.setUniformValue((uniformName + ".quadratic").c_str(),  light.quadratic);
  shader.setUniformValue((uniformName + ".ambient").c_str(),    light.ambient);
  shader.setUniformValue((uniformName + ".diffuse").c_str(),    light.diffuse);
  shader.setUniformValue((uniformName + ".specular").c_str(),   light.specular);
}