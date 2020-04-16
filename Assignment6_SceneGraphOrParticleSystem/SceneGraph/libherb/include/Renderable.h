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

  QVector<QVector3D> m_positions;
  QVector<QVector3D> m_normals;
  QVector<QVector2D> m_texCoords;
  QVector<QVector3D> m_tangents;
  QVector<QVector3D> m_bitangents;
  QVector<unsigned int> m_indexes;
  QString m_textureFile;
  QString m_normalMap;

  /**
   * @brief Create the shaders this will used.
   *
   * Override this to change the shaders.
   */
  virtual void createShaders();

  /**
   * @brief Draw call
   *
   * Override this to change the draw call.
   */
  virtual void drawCall() const;

public:
  Renderable();
  virtual ~Renderable();

  void setPositions(const QVector<QVector3D>& positions) { m_positions = positions; }
  void setNormals(const QVector<QVector3D>& normals) { m_normals = normals; }
  void setTexCoords(const QVector<QVector2D>& texCoords) { m_texCoords = texCoords; }
  void setTangents(const QVector<QVector3D>& tangents) { m_tangents = tangents; }
  void setBitangents(const QVector<QVector3D>& bitangents) { m_bitangents = bitangents; }
  void setIndexes(const QVector<unsigned int>& indexes) { m_indexes = indexes; }
  void setTextureFile(const QString& textureFile) { m_textureFile = textureFile; }
  void setNormalMap(const QString& normalMap) { m_normalMap = normalMap; }

  // WARNING! if you use this method you should use all the above set methods first!
  virtual void init();

  // When we initialize our renderable, we pass it normals.  We
  // currently don't use normals in our implementation, but the array is checked
  // for the appropriate size.  The values can be all 0, but must be the same
  // size as the position array!
  virtual void init(const QVector<QVector3D>& positions,
                    const QVector<QVector3D>& normals,
                    const QVector<QVector2D>& texCoords,
                    const QVector<QVector3D>& tangents,
                    const QVector<QVector3D>& bitangents,
                    const QVector<unsigned int>& indexes,
                    const QString& textureFile, const QString& normalMap);
  virtual void update(const qint64 msSinceLastFrame);
  virtual void draw(const QMatrix4x4& world, const QMatrix4x4& view,
                    const QMatrix4x4& projection,
                    const QVector<Light*>& lights);

  void setModelMatrix(const QMatrix4x4& transform);
  void setRotationAxis(const QVector3D& axis);
  void setRotationSpeed(float speed);

private:
};