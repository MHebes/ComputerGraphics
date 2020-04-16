#include "TexturedQuad.h"

#include "QVector"
#include "QVector2D"
#include "QVector3D"

#include <QtOpenGL>
#include <QOpenGLExtraFunctions>


#define DEFAULT_NORMAL_MAP "@DEFAULT_NORMAL_MAP@"  // CMake var

TexturedQuad::TexturedQuad()
    : m_positions(),
      m_normals(),
      m_texCoords(),
      m_tangents(),
      m_bitangents(),
      m_indices()
{
  m_positions << QVector3D(-0.5, -0.5, 0) << QVector3D(-0.5, 0.5, 0)
              << QVector3D(0.5, 0.5, 0) << QVector3D(0.5, -0.5, 0);

  QVector3D out(0, 0, 1);
  m_normals << out << out << out << out;

  m_texCoords << QVector2D(0, 0) << QVector2D(0, 1) << QVector2D(1, 1)
              << QVector2D(1, 0);

  QVector3D right(1, 0, 0);
  m_tangents << right << right << right << right;

  QVector3D up(0, 1, 0);
  m_bitangents << up << up << up << up;

  m_indices << 0 << 1 << 2 << 2 << 3 << 0;
}

void TexturedQuad::createShaders() {
  Renderable::createShaders();
}

void TexturedQuad::drawCall() const
{
  QOpenGLExtraFunctions::glDrawElementsInstanced(GL_TRIANGLES, m_numTris * 3, GL_UNSIGNED_INT, 0, 1);
}

void TexturedQuad::init(std::string texture)
{
  QString qtexture = QString::fromStdString(texture);
  QString qnormalmap = QString::fromStdString(DEFAULT_NORMAL_MAP);
  Renderable::init(m_positions, m_normals, m_texCoords, m_tangents,
                   m_bitangents, m_indices, qtexture, qnormalmap);
}