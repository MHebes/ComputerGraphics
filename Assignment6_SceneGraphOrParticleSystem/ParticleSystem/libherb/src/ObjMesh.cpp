#include "ObjMesh.h"

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

ObjMesh::ObjMesh()
    : Renderable(),
      m_obj(),
      m_pos(),
      m_norm(),
      m_texCoord(),
      m_tangents(),
      m_bitangents(),
      m_idx()
{
  qDebug() << QDir::currentPath();

  // TODO(mike) better error handling

}

void ObjMesh::init(std::string filename)
{
  // parse obj, get texture file name
  if (m_obj.parse_file(filename) != EXIT_SUCCESS) {
    std::cout << "Could not read file " << filename << std::endl;
    exit(1);
  }

  MtlLoader mtl;
  std::cout << "mtllib = " << m_obj.get_mtllib() << std::endl;
  mtl.parse_file(m_obj.get_mtllib());
  std::cout << "tex = " << mtl.get_map_Kd() << std::endl;
  std::cout << "norm = " << mtl.get_map_Bump() << std::endl;

  m_pos = m_obj.get_vertices();
  m_norm = m_obj.get_normals();
  m_texCoord = m_obj.get_uvs();
  m_idx = m_obj.get_indices();

  m_tangents;
  m_bitangents;

  Util::CalculateTangents(m_pos, m_norm, m_texCoord, m_idx, m_tangents, m_bitangents);

  QString diffuseMap = QString::fromStdString(mtl.get_map_Kd());
  QString normalMap = QString::fromStdString(mtl.get_map_Bump());

  Renderable::init(m_pos, m_norm, m_texCoord, m_tangents, m_bitangents, m_idx, diffuseMap, normalMap);
}