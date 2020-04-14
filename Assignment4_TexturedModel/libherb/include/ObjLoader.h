#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <optional>

#include <QVector3D>
#include <QVector2D>

// face is 9 indices: v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
typedef struct triface {
  bool present[6];  //    vt1,vn1,    vt2,vn2,    vt3,vn3
  int idx[9];       // v1,vt1,vn1, v2,vt2,vn2, v3,vt3,vn3
} triface;

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
  std::vector<QVector3D> get_normals() const { return m_normals; }
  std::vector<QVector3D> get_vertices() const { return m_vertices; }
  std::vector<QVector2D> get_uvs() const { return m_uvs; }
  std::vector<unsigned int> get_indices() const { return m_indices; }

  std::string get_mtllib() const { return m_mtllib; }

  bool const uses_uvs() const { return m_uses_uvs; }
  bool const uses_normals() const { return m_uses_normals; }

private:
  std::vector<QVector3D> m_normals;
  std::vector<QVector3D> m_vertices;
  std::vector<QVector2D> m_uvs;
  std::vector<unsigned int> m_indices;
  std::string m_mtllib;
  bool m_uses_uvs;
  bool m_uses_normals;
};

std::ostream& operator<<(std::ostream& os, const ObjLoader& loader);
std::ostream& operator<<(std::ostream& os, const triface& f);
