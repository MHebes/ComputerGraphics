#include "ObjLoader.h"

#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

ObjLoader::ObjLoader() : m_normals(), m_vertices(), m_faces() {}

std::ostream& operator<<(std::ostream& os, const face& f)
{
  bool begin = true;
  for (auto pair : f) {
    if (!begin) os << ' ';
    begin = false;
    os << pair.first << "//" << pair.second;
  }

  return os;
}

std::ostream& operator<<(std::ostream& os, const ObjLoader& loader)
{
  const std::vector<float> vertices = loader.get_vertices();
  const std::vector<float> normals = loader.get_normals();
  const std::vector<face> faces = loader.get_faces();

  for (int i = 0; i < vertices.size(); i+=3) {
    os << "v " << vertices.at(i) << " " << vertices.at(i+1) << " " << vertices.at(i+2) << std::endl;
  }
  for (int i = 0; i < normals.size(); i+=3) {
    os << "vn " << normals.at(i) << " " << normals.at(i+1) << " " << normals.at(i+2) << std::endl;
  }
  for (const face& f : faces) {
    os << "f " << f << std::endl;
  }

  return os;
}

int ObjLoader::parse_file(const std::string filename)
{
  m_vertices.clear();
  m_normals.clear();
  m_faces.clear();
  m_idx.clear();

  std::ifstream infile(filename);

  if (!infile.is_open()) {
    return EXIT_FAILURE;
  }

  std::string line;
  std::istringstream ss;

  std::string type;  // should be v, vn, f, etc.

  while (std::getline(infile, line)) {
    ss.str(line);

    ss >> type;

    if (type == "v") {  // vertex
      float x;
      float y;
      float z;

      ss >> x >> y >> z;

      m_vertices.reserve(m_vertices.size() + 3);
      m_vertices.push_back(x);
      m_vertices.push_back(y);
      m_vertices.push_back(z);
    }
    else if (type == "vn") {  // normal
      float x;
      float y;
      float z;

      ss >> x >> y >> z;

      m_normals.reserve(m_normals.size() + 3);
      m_normals.push_back(x);
      m_normals.push_back(y);
      m_normals.push_back(z);
    }
    else if (type == "f") {  // face
      face f;

      size_t first;
      size_t second;

      while (ss >> first) {
        if (ss.peek() == '/')
          ss.ignore();
        else
          break;
        if (ss.peek() == '/')
          ss.ignore();
        else
          break;

        ss >> second;

        // stores face as indices into m_vertices and m_normals vectors
        f.emplace_back(first - 1, second - 1);
      }

      ss.clear();

      if (f.size() >= 3) {
        m_faces.push_back(f);
        for (int i = 0; i < 3; i++) {  // TODO support more than 3 m_vertices
          m_idx.push_back(f.at(i).first);
        }
      }
    }
  }

  return EXIT_SUCCESS;
}
