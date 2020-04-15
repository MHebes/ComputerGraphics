#pragma once

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>

#include "Light.h"

class Renderable {
protected:
  // Each renderable has its own model matrix
  QMatrix4x4 m_modelMatrix;

  // For now, we have only one shader per object
  QOpenGLShaderProgram m_shader;

  QOpenGLTexture m_texture;
  QOpenGLTexture m_normalmap;

  QOpenGLBuffer m_vbo;
  QOpenGLBuffer m_ibo;
  QOpenGLVertexArrayObject m_vao;

  // Keep track of how many triangles we actually have to draw in our ibo
  unsigned int m_numTris;
  int m_vertexSize;

  // Define our axis of rotation for animation
  QVector3D m_rotationAxis;
  float m_rotationSpeed;
  float m_rotationAngle;

  // Create our shader and fix it up
  void createShaders();

public:
  Renderable();
  virtual ~Renderable();

  // When we initialize our renderable, we pass it normals.  We
  // currently don't use normals in our implementation, but the array is checked
  // for the appropriate size.  The values can be all 0, but must be the same
  // size as the position array!
  virtual void init(const QVector<QVector3D>& positions,
                    const QVector<QVector3D>& normals,
                    const QVector<QVector2D>& texCoords,
                    const QVector<unsigned int>& indexes,
                    const QString& textureFile, const QString& normalMap,
                    bool flipTextureH = false, bool flipTextureV = false);
  virtual void update(const qint64 msSinceLastFrame);
  virtual void draw(const QMatrix4x4& view, const QMatrix4x4& projection,
                    const std::vector<Light>& lights);

  void setModelMatrix(const QMatrix4x4& transform);
  void setRotationAxis(const QVector3D& axis);
  void setRotationSpeed(float speed);

private:
};