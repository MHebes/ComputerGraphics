#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <optional>

#include <QVector>
#include <QVector3D>
#include <QVector2D>

/**
 * @brief Simple class for extracting data from .obj files.
 *
 */
class ObjLoader {
public:
  ObjLoader();

  /**
   * Loads a .obj file and throws out the old one.
   *
   * @param filename  the filename
   * @return  EXIT_SUCCESS on success
   */
  int parse_file(const std::string filename);

  /**
   * returns a contiguous list of floats - every triplet represents a
   * vertex
   */
  QVector<QVector3D> get_normals() const { return m_normals; }
  QVector<QVector3D> get_vertices() const { return m_vertices; }
  QVector<QVector2D> get_uvs() const { return m_uvs; }
  QVector<unsigned int> get_indices() const { return m_indices; }

  std::string get_mtllib() const { return m_basedir + m_mtllib; }

  bool const uses_uvs() const { return m_uses_uvs; }
  bool const uses_normals() const { return m_uses_normals; }

  // for debugging
  void print();

private:
  QVector<QVector3D> m_normals;
  QVector<QVector3D> m_vertices;
  QVector<QVector2D> m_uvs;
  QVector<unsigned int> m_indices;
  std::string m_mtllib;
  std::string m_basedir;
  bool m_uses_uvs;
  bool m_uses_normals;
};
