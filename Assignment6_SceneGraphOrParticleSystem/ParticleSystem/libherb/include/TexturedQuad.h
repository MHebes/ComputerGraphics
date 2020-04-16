#pragma once

#include <QVector>
#include <QVector3D>
#include <QVector2D>

#include "Renderable.h"

class TexturedQuad : public Renderable {
public:
  TexturedQuad();

  /**
   * @brief Initialize this texture with a texture
   * 
   * @param texture path to file
   */
  void init(std::string texture);

protected:

  virtual void createShaders() override;

  // Override to use glDrawArrayInstance
  virtual void drawCall() const override;

private:
  QVector<QVector3D> m_positions;
  QVector<QVector3D> m_normals;
  QVector<QVector2D> m_texCoords;
  QVector<QVector3D> m_tangents;
  QVector<QVector3D> m_bitangents;
  QVector<unsigned int> m_indices;
};