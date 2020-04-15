#pragma once

#include <QOpenGLShaderProgram>
#include <QVector3D>

/**
 * @brief PointLight
 *
 */
class Light {
public:
  QVector3D position;
  float ambientIntensity;
  float diffuseIntensity;
  float specularIntensity;
  float constant;
  float linear;
  float quadratic;
  QVector3D ambient;
  QVector3D diffuse;
  QVector3D specular;

  Light();

  virtual void applyToUniform(QOpenGLShaderProgram& shader,
                              std::string uniform) const;

  // Tries to approximate ok values for constant, linear, and quadratic
  void setRange(float range);
};