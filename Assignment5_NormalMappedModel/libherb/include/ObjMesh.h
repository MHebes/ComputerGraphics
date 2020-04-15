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

class ObjMesh : public Renderable {
public:
  ObjMesh(std::string filename, float x, float y, float z, float rx, float ry,
          float rz, float rot_speed)
      : Renderable(), m_obj()
  {
    qDebug() << QDir::currentPath();

    // TODO(mike) error handling

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

    QVector<QVector3D> pos = m_obj.get_vertices();
    QVector<QVector3D> norm = m_obj.get_normals();
    QVector<QVector2D> texCoord = m_obj.get_uvs();
    QVector<unsigned int> idx = m_obj.get_indices();

    init(pos, norm, texCoord, idx, QString::fromStdString(mtl.get_map_Kd()),
         QString::fromStdString(mtl.get_map_Bump()), true, true);

    QMatrix4x4 mat;
    mat.setToIdentity();
    mat.translate(x, y, z);
    setModelMatrix(mat);
    setRotationSpeed(rot_speed);

    QVector3D axis(rx, ry, rz);
    axis.normalize();
    setRotationAxis(axis);
  }

private:
  ObjLoader m_obj;
};