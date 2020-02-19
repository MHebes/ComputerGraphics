#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

typedef std::vector<std::pair<unsigned int, unsigned int>> face;

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
  std::vector<float> get_normals() const { return m_normals; }
  std::vector<float> get_vertices() const { return m_vertices; }

  std::vector<face> get_faces() const { return m_faces; }
  std::vector<unsigned int> get_idx() const { return m_idx; }

private:
  std::vector<float> m_normals;
  std::vector<float> m_vertices;
  std::vector<face> m_faces;
  std::vector<unsigned int> m_idx;
};

std::ostream& operator<<(std::ostream& os, const ObjLoader& loader);
std::ostream& operator<<(std::ostream& os, const face& f);
