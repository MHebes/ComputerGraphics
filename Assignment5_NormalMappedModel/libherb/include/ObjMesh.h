#pragma once

#include <QDebug>
#include <QDir>
#include <QMatrix4x4>
#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QVector>
#include <iostream>
#include <string>

#include "MtlLoader.h"
#include "ObjLoader.h"
#include "Renderable.h"
#include "Util.h"

class ObjMesh : public Renderable {
public:
  ObjMesh();

  // Calls initialize on the parent Renderable
  void init(std::string filename);

private:
  ObjLoader m_obj;
  QVector<QVector3D> m_pos;
  QVector<QVector3D> m_norm;
  QVector<QVector2D> m_texCoord;
  QVector<QVector3D> m_tangents;
  QVector<QVector3D> m_bitangents;
  QVector<unsigned int> m_idx;
};